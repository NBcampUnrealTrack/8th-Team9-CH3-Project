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
class UInventorySlotWidget;
class UTextBlock;

UCLASS()
class IAMLEGEND_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void RefreshInventory(const TArray<FItemSlot>& Inventory);
    
    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateDateTime();

protected:

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Date;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Time;
    
	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* InventoryGrid;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UInventorySlotWidget> SlotWidgetClass;
};
