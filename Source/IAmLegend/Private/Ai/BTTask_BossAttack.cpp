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

    if (!Boss->bIsAttacking && !bExtraActionTriggered)
    {
        bExtraActionTriggered = true;

        bool bDoJumpAttack = FMath::RandBool();
        bool bDoScream = FMath::RandBool();

        if (!bDoJumpAttack && !bDoScream)
        {
            AIC->ClearFocus(EAIFocusPriority::Gameplay);
            FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
            return;
        }

        // ? 버그2 수정: &OwnerComp 레퍼런스 캡처 → UBehaviorTreeComponent* 포인터로 교체
        UBehaviorTreeComponent* BTComp = &OwnerComp;
        Boss->PlayExtraActions(bDoJumpAttack, bDoScream, [this, BTComp, AIC]()
            {
                if (!BTComp || !AIC) return;  // 유효성 체크 추가
                AIC->ClearFocus(EAIFocusPriority::Gameplay);
                FinishLatentTask(*BTComp, EBTNodeResult::Succeeded);
            });
    }
}