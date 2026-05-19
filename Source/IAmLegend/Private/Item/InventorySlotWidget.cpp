// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/InventorySlotWidget.h"
#include "Item/UseItemDataAsset.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UInventorySlotWidget::SetSlotData(const FItemSlot& SlotData)
{
	if (SlotData.ItemData)
	{
		
		ItemIcon->SetVisibility(ESlateVisibility::Visible);
		Quantity->SetVisibility(ESlateVisibility::HitTestInvisible);

		ItemIcon->SetBrushFromTexture(SlotData.ItemData->ItemIcon);
		Quantity->SetText(FText::AsNumber(SlotData.Quantity));
	}
	else
	{
		ClearSlot();
	}
}

void UInventorySlotWidget::ClearSlot()
{
	ItemIcon->SetBrushFromTexture(nullptr);
	Quantity->SetText(FText::GetEmpty());
    
	
	ItemIcon->SetVisibility(ESlateVisibility::Collapsed);
	Quantity->SetVisibility(ESlateVisibility::Collapsed);
}