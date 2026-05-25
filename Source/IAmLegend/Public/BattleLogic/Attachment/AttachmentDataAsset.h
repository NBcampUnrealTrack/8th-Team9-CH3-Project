// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/ItemDataAsset.h"
#include "BattleLogic/WeaponBase.h"
#include "AttachmentDataAsset.generated.h"

UENUM(BlueprintType)
enum class EAttachmentSlot : uint8
{
	Muzzle       UMETA(DisplayName = "Muzzle"),       
	Sight        UMETA(DisplayName = "Sight"),        
	Grip		 UMETA(DisplayName = "Grip"),     
	Magazine     UMETA(DisplayName = "Magazine"),     
	Stock        UMETA(DisplayName = "Stock"),        
	None         UMETA(DisplayName = "None")          
};

/**
 * 
 */
UCLASS()
class IAMLEGEND_API UAttachmentDataAsset : public UItemDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attachment|Info")
	EAttachmentSlot AttachmentSlot = EAttachmentSlot::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attachment|Info")
	TArray<EWeaponType> WeaponTypes = { EWeaponType::None };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attachment|Visual")
	FName SocketName; // 부착물이 장착될 소켓 이름 (예: "MuzzleSocket", "SightSocket" 등)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attachment|Stats")
	float SpreadPerShotModifier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attachment|Stats")
	float RecoverySpreadMultiplier = 1.0f; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attachment|Stats")
	float MaxSpreadMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attachment|Stats")
	int32 MaxAmmoBonus = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attachment|Stats")
	float VerticalRecoilModifier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attachment|Stats")
	float HorizontalRecoilModifier = 1.0f;
	
};
