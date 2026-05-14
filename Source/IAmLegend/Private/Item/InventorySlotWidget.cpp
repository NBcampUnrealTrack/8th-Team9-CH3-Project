// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/InventorySlotWidget.h"
#include "Item/UseItemDataAsset.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UInventorySlotWidget::SetSlotData(const FItemSlot& SlotData)
{
	if (SlotData.ItemData)
	{
		ItemIcon->SetBrushFromTexture(
			SlotData.ItemData->ItemIcon
		);

		Quantity->SetText(
			FText::AsNumber(SlotData.Quantity)
		);
	}
	else
	{
		ItemIcon->SetBrushFromTexture(nullptr);

		Quantity->SetText(FText::GetEmpty());
	}
}