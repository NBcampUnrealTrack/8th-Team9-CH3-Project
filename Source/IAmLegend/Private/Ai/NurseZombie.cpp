#include "Ai/NurseZombie.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/HanPlayerCharacter.h"

ANurseZombie::ANurseZombie()
{
    AttackCooldown = 4.0f;
    Health = 100.0f;
    AttackRange = 100.0f;
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
    if (ScreamMontage && CurrentState == EZombieState::Idle)
    {
        SetCurrentState(EZombieState::Screaming);
        bLostSightDuringScream = false;

        if (auto* AIC = Cast<AAIController>(GetController()))
            AIC->StopMovement();

        PlayAnimMontage(ScreamMontage);
    }
}

void ANurseZombie::OnScreamMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (Montage != ScreamMontage) return;
    if (CurrentState != EZombieState::Screaming) return;

    if (bLostSightDuringScream)
    {
        // 비명 중 시야 상실 → TargetActor 비우고 Idle
        AAIController* AIC = Cast<AAIController>(GetController());
        if (AIC && AIC->GetBlackboardComponent())
        {
            AIC->GetBlackboardComponent()->SetValueAsObject(
                TEXT("TargetActor"), nullptr);
        }
        SetCurrentState(EZombieState::Idle);
    }
    else
    {
        // 비명 끝, 플레이어 아직 보임 → TargetActor 먼저 설정 후 Idle
        AAIController* AIC = Cast<AAIController>(GetController());
        if (AIC && AIC->GetBlackboardComponent())
        {
            // ✅ TargetActor 먼저 설정
            AIC->GetBlackboardComponent()->SetValueAsObject(
                TEXT("TargetActor"), PlayerCharacter);
        }

        // ✅ 이동속도 복구
        if (GetCharacterMovement())
            GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;

        // ✅ 마지막에 Idle로 바꿔서 BT가 추격 태스크를 타게 함
        SetCurrentState(EZombieState::Idle);
    }
}

float ANurseZombie::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
    class AController* EventInstigator, AActor* DamageCauser)
{
    if (CurrentState == EZombieState::Dead) return 0.0f;

    if (CurrentState == EZombieState::Screaming)
    {
        StopAnimMontage(ScreamMontage);
        SetCurrentState(EZombieState::Idle);
    }

    return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
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