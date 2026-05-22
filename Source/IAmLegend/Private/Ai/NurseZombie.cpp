#include "Ai/NurseZombie.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BrainComponent.h"
#include "Character/HanPlayerCharacter.h"

ANurseZombie::ANurseZombie()
{
    AttackCooldown = 4.0f;
    Health = 100.0f;
    AttackRange = 200.0f;

    GetCharacterMovement()->MaxWalkSpeed = 150.0f;
}

void ANurseZombie::BeginPlay()
{
    Super::BeginPlay();

    UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
    if (AnimInst)
    {
        AnimInst->OnMontageEnded.AddDynamic(this, &ANurseZombie::OnScreamMontageEnded);
    }
}

void ANurseZombie::Tick(float DeltaTime)
{
    if (CurrentState == EZombieState::Screaming)
    {
        return;
    }
    Super::Tick(DeltaTime);
}

void ANurseZombie::PlayScreamMontage()
{
    if (ScreamMontage && CurrentState == EZombieState::Screaming) return; // 중복 방지
    if (ScreamMontage && CurrentState == EZombieState::Idle)
    {
        SetCurrentState(EZombieState::Screaming);
        bLostSightDuringScream = false;

        AAIController* AIC = Cast<AAIController>(GetController());
        if (AIC)
        {
            AIC->StopMovement();
            AIC->GetBrainComponent()->PauseLogic(TEXT("Screaming")); // BT 일시정지
        }

        // 이동속도 0으로 강제 차단
        GetCharacterMovement()->MaxWalkSpeed = 0.0f;

        PlayAnimMontage(ScreamMontage);

        // 틱 타이머 시작
        GetWorld()->GetTimerManager().SetTimer(ScreamSlowTickHandle, [this]()
            {
                ApplyScreamSlow();
            }, 1.0f, true);
    }
}

void ANurseZombie::ApplyScreamSlow()
{

    if (CurrentState == EZombieState::Dead) return;
    if (!IsValid(this)) return;

    if (!PlayerCharacter) return;

    float Distance = FVector::Dist(GetActorLocation(), PlayerCharacter->GetActorLocation());
    if (Distance > ScreamRadius) return;

    AHanPlayerCharacter* Player = Cast<AHanPlayerCharacter>(PlayerCharacter);
    if (!Player) return;

    // 원래 속도 기준으로 50% 감소
    Player->GetCharacterMovement()->MaxWalkSpeed = Player->GetBaseWalkSpeed() * 0.5f;
    Player->GetCharacterMovement()->MaxWalkSpeedCrouched =
        (Player->GetBaseWalkSpeed() / 2.0f) * 0.5f;

    GetWorld()->GetTimerManager().ClearTimer(ScreamSlowTimerHandle);
    GetWorld()->GetTimerManager().SetTimer(ScreamSlowTimerHandle, [this]()
        {
            RestorePlayerSpeed();
        }, ScreamSlowDuration, false);
}

void ANurseZombie::RestorePlayerSpeed()
{
    AHanPlayerCharacter* Player = Cast<AHanPlayerCharacter>(PlayerCharacter);
    if (!Player) return;

    Player->GetCharacterMovement()->MaxWalkSpeed = Player->GetBaseWalkSpeed();
    Player->GetCharacterMovement()->MaxWalkSpeedCrouched =
        Player->GetBaseWalkSpeed() / 2.0f; // CrouchWalkSpeed = BaseWalkSpeed/2
}

void ANurseZombie::OnScreamMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (Montage != ScreamMontage) return;
    if (CurrentState != EZombieState::Screaming) return;

    GetWorld()->GetTimerManager().ClearTimer(ScreamSlowTickHandle);

    // BT 재개
    AAIController* AIC = Cast<AAIController>(GetController());
    if (AIC)
    {
        AIC->GetBrainComponent()->ResumeLogic(TEXT("Screaming"));
    }

    // 이동속도 복구
    GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;

    if (bLostSightDuringScream)
    {
        if (AIC && AIC->GetBlackboardComponent())
        {
            AIC->GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), nullptr);
        }
        SetCurrentState(EZombieState::Idle);
    }
    else
    {
        if (AIC && AIC->GetBlackboardComponent())
        {
            AIC->GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), PlayerCharacter);
        }
        SetCurrentState(EZombieState::Idle);
    }
}

float ANurseZombie::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
    class AController* EventInstigator, AActor* DamageCauser)
{
    if (CurrentState == EZombieState::Dead) return 0.0f;

    if (CurrentState == EZombieState::Screaming)
    {
        // 스크림 타이머 정리
        GetWorld()->GetTimerManager().ClearTimer(ScreamSlowTickHandle);

        StopAnimMontage(ScreamMontage);

        // BT 재개
        AAIController* AIC = Cast<AAIController>(GetController());
        if (AIC && AIC->GetBrainComponent())
        {
            AIC->GetBrainComponent()->ResumeLogic(TEXT("Screaming"));
        }

        // 이동속도 복구
        GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;

        // 블랙보드에 플레이어 타겟 설정 → Chase 유도
        if (AIC && AIC->GetBlackboardComponent())
        {
            AIC->GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), PlayerCharacter);
        }

        SetCurrentState(EZombieState::Idle);
    }

    return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}
void ANurseZombie::Die()
{
    // 스크림 관련 타이머 전부 클리어
    GetWorld()->GetTimerManager().ClearTimer(ScreamSlowTickHandle);
    GetWorld()->GetTimerManager().ClearTimer(ScreamSlowTimerHandle);

    // BT 완전 중단
    AAIController* AIC = Cast<AAIController>(GetController());
    if (AIC && AIC->GetBrainComponent())
    {
        AIC->GetBrainComponent()->StopLogic(TEXT("Dead"));
    }

    // 플레이어 속도 복구 (죽을 때 슬로우 걸려있으면 해제)
    RestorePlayerSpeed();

    // 부모 Die() 호출
    Super::Die();
}
void ANurseZombie::SetCurrentState(EZombieState NewState)
{
    CurrentState = NewState;

    AAIController* AIC = Cast<AAIController>(GetController());
    if (AIC && AIC->GetBlackboardComponent())
    {
        AIC->GetBlackboardComponent()->SetValueAsEnum(
            TEXT("CurrentState"), (uint8)NewState);
    }
}