// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ItemDataAsset.h"
#include "BattleLogic/WeaponBase.h"
#include "WeaponDataAsset.generated.h"

class AWeaponBase;

/**
 * 
 */
UCLASS()
class IAMLEGEND_API UWeaponDataAsset : public UItemDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class AWeaponBase> WeaponActorClass;
	
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Visual")
    TSoftObjectPtr<USkeletalMesh> WeaponSkeletalMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Stats")
    float Damage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Stats")
    float Range;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Info")
    EWeaponType WeaponType;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Info")
    EWeaponSlot WeaponSlot;

    // 애니메이션 몽타주
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Animation")
    TObjectPtr<UAnimMontage> Attack_1_Montage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Animation")
    TObjectPtr<UAnimMontage> Attack_2_Montage;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Animation")
    TObjectPtr<UAnimMontage> Reload_Montage;

	// 무기 UI 아이콘
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Info")
	TObjectPtr<UTexture2D> WeaponIcon;
};
