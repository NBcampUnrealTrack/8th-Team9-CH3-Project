#include "Ai/PoliceZombie.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


APoliceZombie::APoliceZombie()
{
    AttackCooldown = 4.0f; // 공격 쿨타임 현재 4초의 텀을 가지고 공격함
    Health = 100.0f; // 좀비 체력 현재 100
    AttackRange = 150.0f; // 좀비와 플레이어간의 거리 약 15CM거리에 들어서면 공격모션을 취합니다
}

void APoliceZombie::BeginPlay()
{
    Super::BeginPlay();

}


void APoliceZombie::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}
void APoliceZombie::Die()
{
    if (CurrentState == EZombieState::Dead) return;

    CurrentState = EZombieState::Dead;

    if (AAIController* AIC = Cast<AAIController>(GetController()))
    {
        if (AIC->GetBrainComponent())
            AIC->GetBrainComponent()->StopLogic("Dead");
    }

    GetCharacterMovement()->StopMovementImmediately();
    GetCharacterMovement()->DisableMovement();

    GetWorldTimerManager().ClearTimer(AttackTimerHandle);
    GetWorldTimerManager().ClearTimer(DamageTimerHandle);
    GetWorldTimerManager().ClearTimer(IdleSoundTimerHandle);
    GetWorldTimerManager().ClearTimer(HitTimerHandle);

    Super::Die();

    // ✅ Super 호출 후 DeathMontage 직접 재생
    if (DeathMontage)
    {
        float MontageLength = PlayAnimMontage(DeathMontage);

        // 몽타주 끝나면 마지막 포즈 고정
        GetWorld()->GetTimerManager().SetTimer(DeathFreezeTimerHandle, [this]()
            {
                if (GetMesh())
                {
                    GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
                    GetMesh()->SetAnimation(nullptr);
                    GetMesh()->bNoSkeletonUpdate = true;
                }
            }, MontageLength - 0.05f, false);

        // 숨김 처리 타이머 덮어쓰기
        GetWorld()->GetTimerManager().SetTimer(DeathTimerHandle, [this]()
            {
                SetActorHiddenInGame(true);
                SetLifeSpan(0.1f);
            }, MontageLength + 0.5f, false);
    }
}