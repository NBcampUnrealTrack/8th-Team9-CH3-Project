// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleLogic/Weapon/DataAssets/ThrowableWeaponDataAsset.h"
#include "MeleeWeaponDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class IAMLEGEND_API UMeleeWeaponDataAsset : public UThrowableWeaponDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Melee")
	float AttackCooldown;
};
