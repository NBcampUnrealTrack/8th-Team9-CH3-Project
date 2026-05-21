#pragma once
#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_BossChase.generated.h"

UCLASS()
class IAMLEGEND_API UBTTask_BossChase : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_BossChase();

    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    UPROPERTY(EditAnywhere, Category = "Chase")
    float ChaseSpeed = 400.0f;

    // 공격 범위 진입 시 Chase 종료 거리
    UPROPERTY(EditAnywhere, Category = "Chase")
    float StopDistance = 150.0f;
    float ElapsedTime = 0.0f;
};