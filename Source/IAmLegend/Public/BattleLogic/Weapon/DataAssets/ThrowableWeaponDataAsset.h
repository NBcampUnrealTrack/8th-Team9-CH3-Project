// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponDataAsset.h"
#include "ThrowableWeaponDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class IAMLEGEND_API UThrowableWeaponDataAsset : public UWeaponDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Throwable")
	TSubclassOf<class AWeaponProjectileBase> ProjectileClass;
};
