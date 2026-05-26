#include "Ai/BTTask_BossAttack.h"
#include "AI/Boss_PoliceZombie.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_BossAttack::UBTTask_BossAttack()
{
    NodeName = TEXT("Boss Attack");
    bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_BossAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    bExtraActionTriggered = false;

    AAIController* AIC = OwnerComp.GetAIOwner();
    if (!AIC) return EBTNodeResult::Failed;

    ABoss_PoliceZombie* Boss = Cast<ABoss_PoliceZombie>(AIC->GetPawn());
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!Boss || !BB) return EBTNodeResult::Failed;

    AActor* Target = Cast<AActor>(BB->GetValueAsObject(TEXT("TargetActor")));
    if (Target) AIC->SetFocus(Target);

    // ✅ 이전 AttackCooldown 타이머 클리어
    Boss->GetWorldTimerManager().ClearTimer(Boss->AttackTimerHandle);

    Boss->PlayAttackMontage();

    return EBTNodeResult::InProgress;
}

void UBTTask_BossAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AAIController* AIC = OwnerComp.GetAIOwner();
    if (!AIC) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

    ABoss_PoliceZombie* Boss = Cast<ABoss_PoliceZombie>(AIC->GetPawn());
    if (!Boss) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

    if (!Boss->bIsAttacking && !bExtraActionTriggered)
    {
        bExtraActionTriggered = true;

        // ✅ 둘 중 하나만 선택 (JumpAttack 또는 Scream 또는 없음)
        bool bDoScream = (FMath::RandRange(0, 99) < 60);
        bool bDoJumpAttack = (FMath::RandRange(0, 99) < 60);
        // Random == 2 이면 둘 다 false → 추가 액션 없음

        if (!bDoJumpAttack && !bDoScream)
        {
            AIC->ClearFocus(EAIFocusPriority::Gameplay);
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
            return;
        }

        UBehaviorTreeComponent* BTComp = &OwnerComp;
        Boss->PlayExtraActions(bDoJumpAttack, bDoScream, [this, BTComp, AIC]()
            {
                if (!BTComp || !AIC) return;
                AIC->ClearFocus(EAIFocusPriority::Gameplay);
                FinishLatentTask(*BTComp, EBTNodeResult::Succeeded);
            });
    }
}