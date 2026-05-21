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

    bExtraActionTriggered = false; // ← 추가

    AAIController* AIC = OwnerComp.GetAIOwner();
    if (!AIC) return EBTNodeResult::Failed;

    ABoss_PoliceZombie* Boss = Cast<ABoss_PoliceZombie>(AIC->GetPawn());
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!Boss || !BB) return EBTNodeResult::Failed;

    AActor* Target = Cast<AActor>(BB->GetValueAsObject(TEXT("TargetActor")));
    if (Target) AIC->SetFocus(Target);

    Boss->PlayAttackMontage();

    return EBTNodeResult::InProgress;
}

void UBTTask_BossAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AAIController* AIC = OwnerComp.GetAIOwner();
    if (!AIC) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

    ABoss_PoliceZombie* Boss = Cast<ABoss_PoliceZombie>(AIC->GetPawn());
    if (!Boss) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

    // 공격이 끝나면
    if (!Boss->bIsAttacking && !bExtraActionTriggered)
    {
        bExtraActionTriggered = true;

        // 50% 확률로 JumpAttack
        bool bDoJumpAttack = FMath::RandBool();
        // 50% 확률로 Screaming
        bool bDoScream = FMath::RandBool();

        // 둘 다 없으면 바로 Strafe로 복귀
        if (!bDoJumpAttack && !bDoScream)
        {
            AIC->ClearFocus(EAIFocusPriority::Gameplay);
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
            return;
        }

        // JumpAttack → Screaming 순서로 실행
        Boss->PlayExtraActions(bDoJumpAttack, bDoScream, [this, &OwnerComp, AIC]()
            {
                AIC->ClearFocus(EAIFocusPriority::Gameplay);
                FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
            });
    }
}