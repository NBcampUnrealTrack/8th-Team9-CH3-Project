// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"


USTRUCT(BlueprintType)
struct FItemSlot
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UItemDataAsset> ItemData = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Quantity = 0;

	FItemSlot() {}
	FItemSlot(UItemDataAsset* InData, int32 InQuantity) 
		: ItemData(InData), Quantity(InQuantity) {}
};

class UItemDataAsset;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class IAMLEGEND_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();
	
	UFUNCTION()
	bool AddItem(UItemDataAsset* NewItem);
	
	UFUNCTION()
	void UseItem(int32 Index);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FItemSlot> Inventory;
		
	
protected:
	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;
	
	virtual void BeginPlay() override;
};
