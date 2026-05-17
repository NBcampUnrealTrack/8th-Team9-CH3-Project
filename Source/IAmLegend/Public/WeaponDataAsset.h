// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ItemDataAsset.h"
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
	UPROPERTY(EditAnywhere)
	float Damage = 10.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class AWeaponBase> WeaponActorClass;
	
	// WeaponDataAsset.h
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 MagazineSize = 30; // 한 번 장전하면 들어가는 총알 수
	
	//이 무기가 사용할 총알 에셋
	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<UItemDataAsset> RequiredAmmoItem;
};
