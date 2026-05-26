#include "Ai/BTTask_Patrol.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_Patrol::UBTTask_Patrol()
{
    NodeName = TEXT("Patrol");
}

EBTNodeResult::Type UBTTask_Patrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIC = OwnerComp.GetAIOwner();
    if (!AIC) return EBTNodeResult::Failed;

    ACharacter* Zombie = Cast<ACharacter>(AIC->GetPawn());
    if (!Zombie) return EBTNodeResult::Failed;

    // ✅ 순찰 중 걷는 속도로 설정
    Zombie->GetCharacterMovement()->MaxWalkSpeed = 100.0f;

    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Zombie->GetWorld());
    if (!NavSys) return EBTNodeResult::Failed;

    FNavLocation RandomLocation;
    bool bFound = NavSys->GetRandomReachablePointInRadius(
        Zombie->GetActorLocation(), PatrolRadius, RandomLocation);

    if (!bFound) return EBTNodeResult::Failed;

    AIC->MoveToLocation(RandomLocation.Location, 50.0f, true, true);

    return EBTNodeResult::Succeeded;
}