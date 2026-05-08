// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/BaseItemActor.h"
#include "WeaponItemActor.generated.h"

/**
 * 
 */
UCLASS()
class IAMLEGEND_API AWeaponItemActor : public ABaseItemActor
{
	GENERATED_BODY()
	
	
public:
	void Attack();
};
