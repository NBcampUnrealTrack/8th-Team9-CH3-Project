// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItemActor.h"
#include "UseItemActor.generated.h"

/**
 * 
 */
UCLASS()
class IAMLEGEND_API AUseItemActor : public ABaseItemActor
{
	GENERATED_BODY()
	
public:
	virtual void ApplyPickup(ACharacter* Player) override;
};
