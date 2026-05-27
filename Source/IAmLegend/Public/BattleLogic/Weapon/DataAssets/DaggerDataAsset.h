// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleLogic/Weapon/DataAssets/MeleeWeaponDataAsset.h"
#include "DaggerDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class IAMLEGEND_API UDaggerDataAsset : public UMeleeWeaponDataAsset
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Melee|Dagger")
    float StabDamage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Melee|Dagger")
    float StabCooldown;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Melee|Dagger")
    float StabRange;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Melee|Dagger")
    FVector StabBoxExtent;
};
