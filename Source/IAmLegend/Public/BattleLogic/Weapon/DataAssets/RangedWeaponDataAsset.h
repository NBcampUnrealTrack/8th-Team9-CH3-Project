// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponDataAsset.h"
#include "RangedWeaponDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class IAMLEGEND_API URangedWeaponDataAsset : public UWeaponDataAsset
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ranged")
    float FireRate;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ranged")
    int32 MaxAmmo;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ranged")
    float RecoilAmount;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ranged|Spread")
    float SpreadPerShot;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ranged|Spread")
    float BaseSpreadAngle;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ranged|Spread")
    float MaxSpreadAngle;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ranged|Spread")
    float RecoverySpreadSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ranged|Melee")
    float MeleeAttackCooldown;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ranged|Melee")
    FVector MeleeAttackBoxExtent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ranged|Melee")
    float MeleeAttackRange;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ranged")
    FName MuzzleSocketName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ranged|Animation")
    TObjectPtr<UAnimSequence> FireAnimSequence;
};
