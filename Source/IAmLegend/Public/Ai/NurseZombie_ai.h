#pragma once
#include "CoreMinimal.h"
#include "Ai/BaseZombie_Ai.h"
#include "NurseZombie_ai.generated.h"

UCLASS()
class IAMLEGEND_API ANurseZombie_ai : public ABaseZombie_Ai
{
    GENERATED_BODY()

public:
    ANurseZombie_ai();

    virtual void OnTargetDetected(AActor* Actor, FAIStimulus Stimulus) override;
    void ExpandSightAfterDetection();
};