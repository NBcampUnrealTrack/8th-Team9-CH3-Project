// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/BaseItemActor.h"
#include "TorchItemActor.generated.h"

/**
 * 
 */
UCLASS()
class IAMLEGEND_API ATorchItemActor : public ABaseItemActor
{
	GENERATED_BODY()
	
public:
	ATorchItemActor();

protected:
	// 불빛 역할을 할 SpotLight 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effects")
	TObjectPtr<class USpotLightComponent> FlashlightBeam;
	
public:
	void ToggleTorch(bool bIsOn);
};
