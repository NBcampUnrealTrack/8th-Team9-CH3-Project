// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleLogic/Weapon/DataAssets/RangedWeaponDataAsset.h"
#include "PistolDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class IAMLEGEND_API UPistolDataAsset : public URangedWeaponDataAsset
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Pistol|Melee")
    TSoftObjectPtr<USkeletalMesh> DaggerSkeletalMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Pistol|Melee")
    FName DaggerSocketName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Pistol|Melee")
    float MeleeDamage;
};
