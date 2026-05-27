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
    BellyHitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BellyHitBox"));
    BellyHitBox->SetupAttachment(GetMesh(), FName("Spine1"));
    BellyHitBox->SetBoxExtent(FVector(20.0f, 20.0f, 20.0f));
    BellyHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    BellyHitBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    BellyHitBox->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);

    LegHitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("LegHitBox"));
    LegHitBox->SetupAttachment(GetMesh(), FName("LeftUpLeg"));
    LegHitBox->SetBoxExtent(FVector(20.0f, 20.0f, 20.0f));
    LegHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    LegHitBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    LegHitBox->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);

    GetCharacterMovement()->MaxWalkSpeed = 400.0f;
    GetCharacterMovement()->bUseRVOAvoidance = false;
    AttackCooldown = 4.0f;
    Health = 200.0f;
    AttackRange = 250.0f;
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
    BiteLocation = GetActorLocation();

    FTimerHandle FindPlayerTimer;
    GetWorld()->GetTimerManager().SetTimer(FindPlayerTimer, [this]()
        {
            PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
        }, 0.5f, false);

    BellyHitBox->OnComponentBeginOverlap.AddDynamic(this, &ABoss_PoliceZombie::OnBellyHit);
    LegHitBox->OnComponentBeginOverlap.AddDynamic(this, &ABoss_PoliceZombie::OnLegHit);

    AAIController* AIC = Cast<AAIController>(GetController());
    if (AIC)
    {
        AIC->GetBrainComponent()->PauseLogic(TEXT("Prologue"));
    }

    StartBiteLoop();
}

void ABoss_PoliceZombie::JumpAttackLand()
{
    // Dead 상태면 착지 데미지 차단
    if (CurrentState == EZombieState::Dead) return;
    if (!PlayerCharacter) return;

    float Distance = FVector::Dist(GetActorLocation(), PlayerCharacter->GetActorLocation());
    if (Distance > JumpAttackRadius) return;

    AHanPlayerCharacter* Player = Cast<AHanPlayerCharacter>(PlayerCharacter);
    if (!Player) return;

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
    // Dead 상태면 Bite 차단
    if (CurrentState == EZombieState::Dead) return;

    if (BiteMontage && !bPrologueDone)
    {
        float MontageLength = PlayAnimMontage(BiteMontage);
    }
}

void ABoss_PoliceZombie::ApplyScreamEffect()
{
    // Dead 상태면 스크림 효과 차단
    if (CurrentState == EZombieState::Dead) return;
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

    float OriginalWalkSpeed = Player->GetCharacterMovement()->MaxWalkSpeed;
    float OriginalCrouchSpeed = Player->GetCharacterMovement()->MaxWalkSpeedCrouched;

    Player->GetCharacterMovement()->MaxWalkSpeed *= 0.5f;
    Player->GetCharacterMovement()->MaxWalkSpeedCrouched *= 0.5f;

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

    if (!bPrologueDone)
    {
        bPrologueDone = true;
        OnPrologueTakeDamage();
        return DamageAmount;
    }

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
    GetWorldTimerManager().ClearTimer(Phase2TimerHandle);
    GetWorldTimerManager().ClearTimer(ScreamTimerHandle);
    GetWorldTimerManager().ClearTimer(ExtraActionTimerHandle);
    GetWorldTimerManager().ClearTimer(GroggyTimerHandle);

    AAIController* AIC = Cast<AAIController>(GetController());
    if (AIC)
    {
        AIC->StopMovement();
    }

    if (CrawlMontage)
    {
        StopAnimMontage(CrawlMontage);
    }

    Super::Die();
}

void ABoss_PoliceZombie::OnPrologueTakeDamage()
{
    StopAnimMontage(BiteMontage);
    bIsAttacking = true;

    float CriticalHitLength = 0.0f;
    if (CriticalHitMontage)
    {
        CriticalHitLength = PlayAnimMontage(CriticalHitMontage);
    }

    GetWorld()->GetTimerManager().SetTimer(ScreamTimerHandle, [this]()
        {
            // Dead 상태면 프롤로그 체인 중단
            if (CurrentState == EZombieState::Dead) return;

            AAIController* AIC = Cast<AAIController>(GetController());
            if (AIC && PlayerCharacter)
            {
                AIC->SetFocus(PlayerCharacter);
            }

            float IdleMontageLength = 0.0f;
            if (Idle2Montage)
            {
                IdleMontageLength = PlayAnimMontage(Idle2Montage);
            }

            FTimerHandle IdleTimer;
            GetWorld()->GetTimerManager().SetTimer(IdleTimer, [this]()
                {
                    // Dead 상태면 프롤로그 체인 중단
                    if (CurrentState == EZombieState::Dead) return;

                    float BoundLength = 0.0f;
                    if (BoundMontage)
                    {
                        AAIController* AIC2 = Cast<AAIController>(GetController());
                        if (AIC2)
                        {
                            AIC2->ClearFocus(EAIFocusPriority::Gameplay);
                        }
                        BoundLength = PlayAnimMontage(BoundMontage);
                    }

                    FTimerHandle BoundTimer;
                    GetWorld()->GetTimerManager().SetTimer(BoundTimer, [this]()
                        {
                            // Dead 상태면 프롤로그 체인 중단
                            if (CurrentState == EZombieState::Dead) return;

                            AAIController* AIC = Cast<AAIController>(GetController());
                            if (AIC && PlayerCharacter)
                            {
                                AIC->SetFocus(PlayerCharacter);
                            }

                            float ScreamLength = 0.0f;
                            if (ScreamMontage)
                            {
                                bIsScreaming = true;
                                ScreamLength = PlayAnimMontage(ScreamMontage);
                                ApplyScreamEffect();
                            }

                            FTimerHandle ScreamTimer;
                            GetWorld()->GetTimerManager().SetTimer(ScreamTimer, [this]()
                                {
                                    // Dead 상태면 전투 시작 차단
                                    if (CurrentState == EZombieState::Dead) return;
                                    StartCombat();
                                }, ScreamLength, false);

                        }, BoundLength, false);

                }, 3.0f, false);

        }, CriticalHitLength, false);
}

void ABoss_PoliceZombie::StartCombat()
{
    // Dead 상태면 전투 시작 차단
    if (CurrentState == EZombieState::Dead) return;

    bCombatStarted = true;
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
    if (CurrentState == EZombieState::Dead) return;

    bIsAttacking = false;

    if (GetCharacterMovement())
    {
        GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking); // ← 추가
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
        AIC->ClearFocus(EAIFocusPriority::Gameplay);
    }
}
void ABoss_PoliceZombie::OnBellyHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (CurrentState == EZombieState::Dead) return;
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
    if (CurrentState == EZombieState::Dead) return;
    if (!OtherActor || OtherActor == this) return;

    LegHitCount++;
    if (LegHitCount >= LegHitThreshold)
    {
        LegHitCount = 0;
    }
}
void ABoss_PoliceZombie::PlayExtraActions(bool bDoJumpAttack, bool bDoScream, TFunction<void()> OnComplete)
{
    if (CurrentState == EZombieState::Dead) return;

    AAIController* AIC = Cast<AAIController>(GetController());
    if (AIC && PlayerCharacter)
    {
        AIC->SetFocus(PlayerCharacter);
    }

    // ✅ 순서: Scream → JumpAttack
    if (bDoScream && ScreamMontage)
    {
        bIsScreaming = true;
        bIsAttacking = true;
        float ScreamLength = PlayAnimMontage(ScreamMontage);
        ApplyScreamEffect();

        GetWorld()->GetTimerManager().SetTimer(ExtraActionTimerHandle, [this, bDoJumpAttack, OnComplete]()
            {
                if (CurrentState == EZombieState::Dead)
                {
                    OnComplete();
                    return;
                }

                bIsScreaming = false;

                // ✅ Scream 후 JumpAttack 실행
                if (bDoJumpAttack && JumpAttackMontage)
                {
                    bIsAttacking = true;
                    float JumpLength = PlayAnimMontage(JumpAttackMontage);

                    GetWorld()->GetTimerManager().SetTimer(ExtraActionTimerHandle, [this, OnComplete]()
                        {
                            if (CurrentState == EZombieState::Dead)
                            {
                                OnComplete();
                                return;
                            }
                            bIsAttacking = false;
                            OnComplete();
                        }, JumpLength, false);
                }
                else
                {
                    bIsAttacking = false;
                    OnComplete();
                }
            }, ScreamLength, false);
    }
    else if (bDoJumpAttack && JumpAttackMontage)
    {
        // ✅ Scream 없이 JumpAttack만 실행
        bIsAttacking = true;
        float JumpLength = PlayAnimMontage(JumpAttackMontage);

        GetWorld()->GetTimerManager().SetTimer(ExtraActionTimerHandle, [this, OnComplete]()
            {
                if (CurrentState == EZombieState::Dead)
                {
                    OnComplete();
                    return;
                }
                bIsAttacking = false;
                OnComplete();
            }, JumpLength, false);
    }
    else
    {
        OnComplete();
    }
}

void ABoss_PoliceZombie::StartPhase2()
{
    // Dead 상태면 페이즈2 차단
    if (CurrentState == EZombieState::Dead) return;

    AAIController* AIC = Cast<AAIController>(GetController());
    if (AIC)
    {
        AIC->GetBrainComponent()->PauseLogic(TEXT("Phase2"));
        AIC->StopMovement();
    }

    bIsAttacking = true;

    float ScreamLength = 0.0f;
    if (ScreamMontage)
    {
        bIsScreaming = true;
        ScreamLength = PlayAnimMontage(ScreamMontage);
        ApplyScreamEffect();
    }

    GetWorld()->GetTimerManager().SetTimer(Phase2TimerHandle, [this]()
        {
            // Dead 상태면 페이즈2 체인 중단
            if (CurrentState == EZombieState::Dead) return;

            bIsScreaming = false;
            float GetDownLength = 0.0f;
            if (DownMontage)
            {
                GetDownLength = PlayAnimMontage(DownMontage);
            }

            GetWorld()->GetTimerManager().SetTimer(Phase2TimerHandle, [this]()
                {
                    // Dead 상태면 페이즈2 체인 중단
                    if (CurrentState == EZombieState::Dead) return;
                    StartCrawlToBiteLocation();
                }, GetDownLength, false);

        }, ScreamLength, false);
}

void ABoss_PoliceZombie::StartCrawlToBiteLocation()
{
    // Dead 상태면 크롤 차단
    if (CurrentState == EZombieState::Dead) return;

    HitMontage = nullptr;

    AAIController* AIC = Cast<AAIController>(GetController());
    if (!AIC) return;

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
            // Dead 상태면 크롤 체크 중단
            if (CurrentState == EZombieState::Dead)
            {
                GetWorldTimerManager().ClearTimer(Phase2TimerHandle);
                return;
            }
            OnCrawlArrived();
        }, 0.5f, true);
}

void ABoss_PoliceZombie::OnCrawlArrived()
{
    // Dead 상태면 차단
    if (CurrentState == EZombieState::Dead) return;

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
    // Dead 상태면 차단
    if (CurrentState == EZombieState::Dead) return;

    AAIController* AIC = Cast<AAIController>(GetController());
    if (AIC) AIC->StopMovement();

    if (BiteMontage)
    {
        PlayAnimMontage(BiteMontage);
    }

    GetWorld()->GetTimerManager().SetTimer(Phase2TimerHandle, [this]()
        {
            // Dead 상태면 페이즈2 종료 차단
            if (CurrentState == EZombieState::Dead) return;
            EndPhase2Bite();
        }, 10.0f, false);
}

void ABoss_PoliceZombie::EndPhase2Bite()
{
    // Dead 상태면 차단
    if (CurrentState == EZombieState::Dead) return;

    Health = FMath::Clamp(Health + (MaxHealth * 0.5f), 0.0f, 100.0f);
    bPhase2Triggered = false;
    StopAnimMontage(BiteMontage);

    float GetUpLength = 0.0f;
    if (GetUpMontage)
    {
        GetUpLength = PlayAnimMontage(GetUpMontage);
    }

    GetWorld()->GetTimerManager().SetTimer(Phase2TimerHandle, [this]()
        {
            // Dead 상태면 페이즈2 체인 중단
            if (CurrentState == EZombieState::Dead) return;

            float ScreamLength = 0.0f;
            if (ScreamMontage)
            {
                bIsScreaming = true;
                ScreamLength = PlayAnimMontage(ScreamMontage);
                ApplyScreamEffect();
            }

            GetWorld()->GetTimerManager().SetTimer(Phase2TimerHandle, [this]()
                {
                    // Dead 상태면 전투 재개 차단
                    if (CurrentState == EZombieState::Dead) return;
                    StartCombat();
                }, ScreamLength, false);

        }, GetUpLength, false);
}