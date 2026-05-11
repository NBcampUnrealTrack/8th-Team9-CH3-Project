// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/InventoryComponent.h"
#include "InventorySlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class IAMLEGEND_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta=(BindWidget))
	class UImage* ItemIcon;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Quantity;
	
	void SetSlotData(const FItemSlot& SlotData);
	
	
};
