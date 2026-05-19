#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemDataAsset.generated.h"

// 아이템 카테고리 정의 무기종류 와 아이템 종류
UENUM(BlueprintType)
enum class EItemCategory : uint8
{
	Recovery	 UMETA(DisplayName = "Recovery"),
	Buff		 UMETA(DisplayName = "Buff"),
	Gun          UMETA(DisplayName = "Weapon"),
	Melee        UMETA(DisplayName = "Melee Weapon"),
	Throwable    UMETA(DisplayName = "Throwable"),
	Ammo		 UMETA(DisplayName = "Ammo"),
	CraftItem	 UMETA(DisplayName = "Craft Item"),
	ResultItem   UMETA(DisplayName = "Result Item"),
	Mission      UMETA(DisplayName = "Mission")
};

UCLASS(BlueprintType)
class IAMLEGEND_API UItemDataAsset : public UDataAsset
{
	GENERATED_BODY()  
    
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	EItemCategory Category;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	bool bIsStackable = false; // 투척물이나 포션은 true로 설정 여러개 보유할 수 있는 설정

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
	TObjectPtr<UStaticMesh> ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	TSubclassOf<class ABaseItemActor> ItemActorClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
	TObjectPtr<UTexture2D> ItemIcon;
	
	
	// --- 추가: 메쉬 위치/회전 보정값 ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
	FVector MeshRelativeLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
	FRotator MeshRelativeRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
	float MeshScale = 1.0f; 
	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	int32 MaxStackSize = 999;
};