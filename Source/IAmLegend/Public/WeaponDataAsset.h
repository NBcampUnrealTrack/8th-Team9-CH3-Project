// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ItemDataAsset.h"
#include "WeaponDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class IAMLEGEND_API UWeaponDataAsset : public UItemDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	float Damage = 10.f;
	
};
