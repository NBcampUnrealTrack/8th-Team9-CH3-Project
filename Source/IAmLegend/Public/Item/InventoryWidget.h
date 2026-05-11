// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryComponent.h"
#include "InventoryWidget.generated.h"

/**
 * 
 */

class UUniformGridPanel;
class UInventorySlotWidget;

UCLASS()
class IAMLEGEND_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void RefreshInventory(const TArray<FItemSlot>& Inventory);

protected:

	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* InventoryGrid;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UInventorySlotWidget> SlotWidgetClass;
};
