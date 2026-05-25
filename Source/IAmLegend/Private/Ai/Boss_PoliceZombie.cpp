#include "Ai/Boss_PoliceZombie.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "AIController.h"
#include "Components/SphereComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BrainComponent.h"
#include "NiagaraFunctionLibrary.h"

ABoss_PoliceZombie::ABoss_PoliceZombie()
{
    // 배 히트박스 생성
    BellyHitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BellyHitBox"));
    BellyHitBox->SetupAttachment(GetMesh(), FName("Spine1"));
    BellyHitBox->SetBoxExtent(FVector(20.0f, 20.0f, 20.0f));
    BellyHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    BellyHitBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    BellyHitBox->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);

    // 다리 히트박스 생성
    LegHitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LegHitBox"));
    LegHitBox->SetupAttachment(GetMesh(), FName("LeftUpLeg"));
    LegHitBox->SetBoxExtent(FVector(20.0f, 20.0f, 20.0f));
    LegHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    LegHitBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    LegHitBox->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);

    GetCharacterMovement()->MaxWalkSpeed = 400.0f;
    GetCharacterMovement()->bUseRVOAvoidance = false;
    AttackCooldown = 4.0f; // 공격 쿨타임 현재 4초의 텀을 가지고 공격함
    Health = 200.0f; // 좀비 체력 현재 100
    AttackRange = 250.0f; // 좀비와 플레이어간의 거리 약 15CM거리에 들어서면 공격모션을 취합니다
    MaxHealth = Health;
}
void ABoss_PoliceZombie::Tick(float DeltaTime)
{
    // 보스는 BT에서 공격 관리하므로 부모 Tick 공격 로직 스킵
    ACharacter::Tick(DeltaTime);
}
void ABoss_PoliceZombie::BeginPlay()
{
    Super::BeginPlay();
    GetMesh()->SetWorldScale3D(FVector(1.5f, 1.5f, 1.5f));
    BiteLocation = GetActorLocation();
    UE_LOG(LogTemp, Warning, TEXT("BiteLocation 저장: X=%f, Y=%f, Z=%f"),
        BiteLocation.X, BiteLocation.Y, BiteLocation.Z);

    FTimerHandle FindPlayerTimer;
    GetWorld()->GetTimerManager().SetTimer(FindPlayerTimer, [this]()
        {
            PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
        }, 0.5f, false);

    BellyHitBox->OnComponentBeginOverlap.AddDynamic(this, &ABoss_PoliceZombie::OnBellyHit);
    LegHitBox->OnComponentBeginOverlap.AddDynamic(this, &ABoss_PoliceZombie::OnLegHit);

    // BT 일시정지 + Bite 시작
    AAIController* AIC = Cast<AAIController>(GetController());
    if (AIC)
    {
        AIC->GetBrainComponent()->PauseLogic(TEXT("Prologue"));
    }

    // Bite 몽타주 반복
    StartBiteLoop();
}


void ABoss_PoliceZombie::JumpAttackLand()
{
    if (!PlayerCharacter) return;

    float Distance = FVector::Dist(GetActorLocation(), PlayerCharacter->GetActorLocation());
    if (Distance > JumpAttackRadius) return;

    AHanPlayerCharacter* Player = Cast<AHanPlayerCharacter>(PlayerCharacter);
    if (!Player) return;

    // ✅ 플레이어가 땅에 닿아있을 때만 데미지
    if (Player->GetCharacterMovement()->IsMovingOnGround() ||
        Player->GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Walking)
    {
        UGameplayStatics::ApplyDamage(Player, JumpAttackDamage, GetController(), this, nullptr);
        UE_LOG(LogTemp, Warning, TEXT("점프 공격 착지 데미지! 거리: %f"), Distance);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("플레이어 점프로 회피 성공! 거리: %f"), Distance);
    }
}
void ABoss_PoliceZombie::StartBiteLoop()
{
    if (BiteMontage && !bPrologueDone)
    {
        float MontageLength = PlayAnimMontage(BiteMontage);
        //FTimerHandle BiteLoopTimer;
        //GetWorld()->GetTimerManager().SetTimer(BiteLoopTimer, [this]()
        //    {
        //        StartBiteLoop();
        //    }, MontageLength, false);
    }
}
void ABoss_PoliceZombie::ApplyScreamEffect()
{
    if (!PlayerCharacter) return;

    AHanPlayerCharacter* Player = Cast<AHanPlayerCharacter>(PlayerCharacter);
    if (!Player) return;
    if (ScreamEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            ScreamEffect,
            GetActorLocation(),
            GetActorRotation()
        );
    }
    // 원래 속도 저장
    float OriginalWalkSpeed = Player->GetCharacterMovement()->MaxWalkSpeed;
    float OriginalCrouchSpeed = Player->GetCharacterMovement()->MaxWalkSpeedCrouched;

    // 속도 감소
    Player->GetCharacterMovement()->MaxWalkSpeed *= 0.5f;
    Player->GetCharacterMovement()->MaxWalkSpeedCrouched *= 0.5f;

    // 5초 후 저장해둔 원래 속도로 복구
    FTimerHandle ScreamEffectTimer;
    GetWorld()->GetTimerManager().SetTimer(ScreamEffectTimer, [Player, OriginalWalkSpeed, OriginalCrouchSpeed]()
        {
            if (Player)
            {
                Player->GetCharacterMovement()->MaxWalkSpeed = OriginalWalkSpeed;
                Player->GetCharacterMovement()->MaxWalkSpeedCrouched = OriginalCrouchSpeed;
            }
        }, 5.0f, false);
}
float ABoss_PoliceZombie::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
    class AController* EventInstigator, AActor* DamageCauser)
{
    if (CurrentState == EZombieState::Dead) return 0.0f;

    // ✅ 첫 피격 → 프롤로그 시작
    if (!bPrologueDone)
    {
        bPrologueDone = true;
        OnPrologueTakeDamage();
        return DamageAmount;
    }

    // ✅ 프롤로그 진행 중 (첫 피격 이후 ~ StartCombat 전) 무적
    if (!bCombatStarted) return 0.0f;

    Health -= DamageAmount;

    UE_LOG(LogTemp, Warning, TEXT("보스 현재 체력: %f / %f"), Health, MaxHealth);

    if (Health <= 0.0f)
    {
        Die();
        return DamageAmount;
    }

    if (!bPhase2Triggered && Health <= MaxHealth * 0.2f)
    {
        bPhase2Triggered = true;
        StartPhase2();
    }

    return DamageAmount;
}
void ABoss_PoliceZombie::Die()
{
    // ✅ 페이즈 2 관련 타이머 전부 정리
    GetWorldTimerManager().ClearTimer(Phase2TimerHandle);
    GetWorldTimerManager().ClearTimer(ScreamTimerHandle);
    GetWorldTimerManager().ClearTimer(ExtraActionTimerHandle);
    GetWorldTimerManager().ClearTimer(GroggyTimerHandle);

    // ✅ 이동 중지
    AAIController* AIC = Cast<AAIController>(GetController());
    if (AIC)
    {
        AIC->StopMovement();
    }

    // ✅ Crawl 몽타주 중단
    if (CrawlMontage)
    {
        StopAnimMontage(CrawlMontage);
    }

    Super::Die();
}

void ABoss_PoliceZombie::OnPrologueTakeDamage()
{
    // Bite 중지
    StopAnimMontage(BiteMontage);
    bIsAttacking = true; // 다른 동작 차단

    // 1. CriticalHit 몽타주 재생
    float CriticalHitLength = 0.0f;
    if (CriticalHitMontage)
    {
        CriticalHitLength = PlayAnimMontage(CriticalHitMontage);
    }

    // 2. CriticalHit 종료 후 Idle 5초 (플레이어 바라보기)
    GetWorld()->GetTimerManager().SetTimer(ScreamTimerHandle, [this]()
        {


            // 플레이어 실시간 바라보기 시작
            AAIController* AIC = Cast<AAIController>(GetController());
            if (AIC && PlayerCharacter)
            {
                AIC->SetFocus(PlayerCharacter);
            }
            // 새 Idle 몽타주 재생
            float IdleMontageLength = 0.0f;
            if (Idle2Montage) // 새 Idle 몽타주
            {
                IdleMontageLength = PlayAnimMontage(Idle2Montage);
            }
            // 5초 후 Bound 몽타주
            FTimerHandle IdleTimer;
            GetWorld()->GetTimerManager().SetTimer(IdleTimer, [this]()
                {
                    // 3. Bound 몽타주 재생
                    float BoundLength = 0.0f;
                    if (BoundMontage)
                    {
                        // Bound 시작 시 Focus 해제
                        AAIController* AIC2 = Cast<AAIController>(GetController());
                        if (AIC2)
                        {
                            AIC2->ClearFocus(EAIFocusPriority::Gameplay);
                        }
                        BoundLength = PlayAnimMontage(BoundMontage);
                    }

                    // 4. Bound 종료 후 Screaming
                    FTimerHandle BoundTimer;
                    GetWorld()->GetTimerManager().SetTimer(BoundTimer, [this]()
                        {

                            AAIController* AIC = Cast<AAIController>(GetController());
                            if (AIC && PlayerCharacter)
                            {
                                AIC->SetFocus(PlayerCharacter);
                            }

                            // Screaming 몽타주
                            float ScreamLength = 0.0f;
                            if (ScreamMontage)
                            {
                                bIsScreaming = true;
                                ScreamLength = PlayAnimMontage(ScreamMontage);
                                ApplyScreamEffect();
                            }

                            // Screaming 종료 후 전투 시작
                            FTimerHandle ScreamTimer;
                            GetWorld()->GetTimerManager().SetTimer(ScreamTimer, [this]()
                                {
                                    StartCombat();
                                }, ScreamLength, false);

                        }, BoundLength, false);

                }, 3.0f, false);

        }, CriticalHitLength, false);
}


void ABoss_PoliceZombie::StartCombat()
{
    bCombatStarted = true; // ✅ 여기서 무적 해제
    bIsScreaming = false;
    bIsAttacking = false;
    CurrentState = EZombieState::Idle;

    AAIController* AIC = Cast<AAIController>(GetController());
    if (AIC)
    {
        AIC->GetBrainComponent()->ResumeLogic(TEXT("Prologue"));
    }
}

void ABoss_PoliceZombie::ResetAttack()
{
    bIsAttacking = false;

    if (GetCharacterMovement())
    {
        GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;
    }

    if (CurrentState != EZombieState::Dead)
    {
        CurrentState = EZombieState::Idle;
    }

    AAIController* AIC = Cast<AAIController>(GetController());
    if (AIC && AIC->GetBlackboardComponent())
    {
        AIC->GetBlackboardComponent()->SetValueAsEnum(TEXT("CurrentState"), (uint8)CurrentState);
    }
    if (AIC)
    {
        AIC->ClearFocus(EAIFocusPriority::Gameplay); // ← 추가
    }
}

void ABoss_PoliceZombie::OnBellyHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == this) return;
    BellyHitCount++;
    if (BellyHitCount >= BellyHitThreshold && !bPhaseTransitioned)
    {
        // 페이즈 전환 로직 (나중에 구현)
    }
}
void ABoss_PoliceZombie::OnLegHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == this) return;

    LegHitCount++;

    if (LegHitCount >= LegHitThreshold)
    {
        // 넘어짐 로직 (나중에 구현)
        LegHitCount = 0;
    }
}
void ABoss_PoliceZombie::PlayExtraActions(bool bDoJumpAttack, bool bDoScream, TFunction<void()> OnComplete)
{
    AAIController* AIC = Cast<AAIController>(GetController());
    if (AIC && PlayerCharacter)
    {
        AIC->SetFocus(PlayerCharacter);
    }

    if (bDoJumpAttack && JumpAttackMontage)
    {
        bIsAttacking = true;
        float JumpLength = PlayAnimMontage(JumpAttackMontage);

        GetWorld()->GetTimerManager().SetTimer(ExtraActionTimerHandle, [this, bDoScream, OnComplete]()
            {
                bIsScreaming = false;
                bIsAttacking = false;

                if (bDoScream && ScreamMontage)
                {
                    bIsScreaming = true;
                    bIsAttacking = true;
                    float ScreamLength = PlayAnimMontage(ScreamMontage);
                    ApplyScreamEffect();

                    GetWorld()->GetTimerManager().SetTimer(ExtraActionTimerHandle, [this, OnComplete]()
                        {
                            bIsAttacking = false;
                            OnComplete();
                        }, ScreamLength, false);
                }
                else
                {
                    OnComplete();
                }
            }, JumpLength, false);
    }
    else if (bDoScream && ScreamMontage)
    {
        bIsScreaming = true;
        bIsAttacking = true;
        float ScreamLength = PlayAnimMontage(ScreamMontage);

        GetWorld()->GetTimerManager().SetTimer(ExtraActionTimerHandle, [this, OnComplete]()
            {
                bIsScreaming = false;
                bIsAttacking = false;
                OnComplete();
            }, ScreamLength, false);
    }
}
void ABoss_PoliceZombie::StartPhase2()
{
    // BT 일시정지
    AAIController* AIC = Cast<AAIController>(GetController());
    if (AIC)
    {
        AIC->GetBrainComponent()->PauseLogic(TEXT("Phase2"));
        AIC->StopMovement();
    }

    bIsAttacking = true;

    // Screaming 몽타주
    float ScreamLength = 0.0f;
    if (ScreamMontage)
    {
        bIsScreaming = true;
        ScreamLength = PlayAnimMontage(ScreamMontage);
        ApplyScreamEffect();
    }

    // Screaming 후 GetDown
    GetWorld()->GetTimerManager().SetTimer(Phase2TimerHandle, [this]()
        {
            bIsScreaming = false;
            float GetDownLength = 0.0f;
            if (DownMontage)
            {
                GetDownLength = PlayAnimMontage(DownMontage);
            }

            GetWorld()->GetTimerManager().SetTimer(Phase2TimerHandle, [this]()
                {
                    StartCrawlToBiteLocation();
                }, GetDownLength, false);

        }, ScreamLength, false);
}

void ABoss_PoliceZombie::StartCrawlToBiteLocation()
{
    HitMontage = nullptr;

    AAIController* AIC = Cast<AAIController>(GetController());
    if (!AIC) return;

    // Focus 해제 후 BiteLocation 방향으로 즉시 회전
    AIC->ClearFocus(EAIFocusPriority::Gameplay);

    FVector ToBiteLocation = (BiteLocation - GetActorLocation()).GetSafeNormal();
    FRotator LookRot = FRotationMatrix::MakeFromX(ToBiteLocation).Rotator();
    LookRot.Pitch = 0.0f;
    LookRot.Roll = 0.0f;
    SetActorRotation(LookRot);

    if (CrawlMontage)
    {
        PlayAnimMontage(CrawlMontage);
        GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
        GetCharacterMovement()->MaxWalkSpeed = 150.0f;
    }

    AIC->MoveToLocation(BiteLocation, 50.0f, true, true);

    GetWorld()->GetTimerManager().SetTimer(Phase2TimerHandle, [this]()
        {
            OnCrawlArrived();
        }, 0.5f, true);
}
void ABoss_PoliceZombie::OnCrawlArrived()
{
    float Distance = FVector::Dist(GetActorLocation(), BiteLocation);
    UE_LOG(LogTemp, Warning, TEXT("Crawl 거리 체크: %f"), Distance);

    if (Distance <= 150.0f)
    {
        UE_LOG(LogTemp, Warning, TEXT("Crawl 도착! Bite 시작"));
        GetWorld()->GetTimerManager().ClearTimer(Phase2TimerHandle);
        StartPhase2Bite();
    }
}
void ABoss_PoliceZombie::StartPhase2Bite()
{
    AAIController* AIC = Cast<AAIController>(GetController());
    if (AIC) AIC->StopMovement();

    // Bite 몽타주 재생
    if (BiteMontage)
    {
        PlayAnimMontage(BiteMontage);
    }

    // 10초 후 페이즈 2 종료
    GetWorld()->GetTimerManager().SetTimer(Phase2TimerHandle, [this]()
        {
            EndPhase2Bite();
        }, 10.0f, false);
}

void ABoss_PoliceZombie::EndPhase2Bite()
{
    // HP 50% 회복
    Health = FMath::Clamp(Health + (MaxHealth * 0.5f), 0.0f, 100.0f);
    bPhase2Triggered = false;
    StopAnimMontage(BiteMontage);

    // GetUp 몽타주
    float GetUpLength = 0.0f;
    if (GetUpMontage)
    {
        GetUpLength = PlayAnimMontage(GetUpMontage);
    }

    // GetUp 후 Screaming → 전투 재개
    GetWorld()->GetTimerManager().SetTimer(Phase2TimerHandle, [this]()
        {
            float ScreamLength = 0.0f;
            if (ScreamMontage)
            {
                bIsScreaming = true;
                ScreamLength = PlayAnimMontage(ScreamMontage);
                ApplyScreamEffect();
            }

            GetWorld()->GetTimerManager().SetTimer(Phase2TimerHandle, [this]()
                {
                    StartCombat();
                }, ScreamLength, false);

        }, GetUpLength, false);
}