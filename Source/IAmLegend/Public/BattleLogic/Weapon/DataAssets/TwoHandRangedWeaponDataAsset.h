// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleLogic/Weapon/DataAssets/RangedWeaponDataAsset.h"
#include "TwoHandRangedWeaponDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class IAMLEGEND_API UTwoHandRangedWeaponDataAsset : public URangedWeaponDataAsset
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|TwoHandRanged") 
    FName LeftHandIKSocketName; 

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|TwoHandRanged|Melee")
    float PushForce;
};
