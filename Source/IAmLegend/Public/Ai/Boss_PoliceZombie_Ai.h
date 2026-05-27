// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ai/BaseZombie_Ai.h"
#include "Boss_PoliceZombie_Ai.generated.h"

/**
 * 
 */
UCLASS()
class IAMLEGEND_API ABoss_PoliceZombie_Ai : public ABaseZombie_Ai
{
	GENERATED_BODY()
	


	ABoss_PoliceZombie_Ai();

	virtual void OnTargetDetected(AActor* Actor, FAIStimulus Stimulus) override;

};
