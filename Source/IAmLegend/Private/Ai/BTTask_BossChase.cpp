#include "Ai/BTTask_BossChase.h"
#include "AI/Boss_PoliceZombie.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_BossChase::UBTTask_BossChase()
{
    NodeName = TEXT("Boss Chase");
    bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_BossChase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIC = OwnerComp.GetAIOwner();
    if (!AIC) return EBTNodeResult::Failed;

    ACharacter* Boss = Cast<ACharacter>(AIC->GetPawn());
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!Boss || !BB) return EBTNodeResult::Failed;

    AActor* Target = Cast<AActor>(BB->GetValueAsObject(TEXT("TargetActor")));
    if (!Target) return EBTNodeResult::Failed;

    Boss->GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
    AIC->SetFocus(Target);
    AIC->MoveToActor(Target, StopDistance, true, true, true);

    return EBTNodeResult::InProgress;
}

void UBTTask_BossChase::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AAIController* AIC = OwnerComp.GetAIOwner();
    if (!AIC) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

    ACharacter* Boss = Cast<ACharacter>(AIC->GetPawn());
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!Boss || !BB) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

    AActor* Target = Cast<AActor>(BB->GetValueAsObject(TEXT("TargetActor")));
    if (!Target) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

    // MoveToActor 매 프레임 대신 일정 간격으로만 갱신
    ElapsedTime += DeltaSeconds;
    if (ElapsedTime >= 0.5f) // 0.5초마다 경로 갱신
    {
        AIC->MoveToActor(Target, StopDistance, true, true, true);
        ElapsedTime = 0.0f;
    }

    // 거리 체크
    ABoss_PoliceZombie* BossZombie = Cast<ABoss_PoliceZombie>(Boss);
    float StopRange = BossZombie ? BossZombie->AttackRange : StopDistance;

    float Distance = FVector::Dist(Boss->GetActorLocation(), Target->GetActorLocation());
    if (Distance <= StopRange)
    {
        AIC->ClearFocus(EAIFocusPriority::Gameplay);
        AIC->StopMovement();
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}