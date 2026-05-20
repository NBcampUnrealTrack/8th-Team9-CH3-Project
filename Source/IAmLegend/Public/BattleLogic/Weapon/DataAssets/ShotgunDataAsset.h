// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleLogic/Weapon/DataAssets/TwoHandRangedWeaponDataAsset.h"
#include "ShotgunDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class IAMLEGEND_API UShotgunDataAsset : public UTwoHandRangedWeaponDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Shotgun")
	int32 PelletsPerShot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Shotgun")
	int32 AmmoPerReload;
};
