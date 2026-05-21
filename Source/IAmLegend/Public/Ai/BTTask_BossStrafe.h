#pragma once
#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_BossStrafe.generated.h"

UCLASS()
class IAMLEGEND_API UBTTask_BossStrafe : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UBTTask_BossStrafe();
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

    UPROPERTY(EditAnywhere, Category = "Strafe")
    float MinStrafeTime = 2.0f;

    UPROPERTY(EditAnywhere, Category = "Strafe")
    float MaxStrafeTime = 5.0f;

    UPROPERTY(EditAnywhere, Category = "Strafe")
    float StrafeSpeed = 200.0f;

private:
    float ElapsedTime = 0.0f;
    float TargetStrafeTime = 0.0f;
    float StrafeDirection = 1.0f;
};