// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/IngredientSlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Item/ItemDataAsset.h"

void UIngredientSlotWidget::InitIngredient(UItemDataAsset* ItemData, int32 RequiredQuantity)
{
	if (!ItemData) return;

	if (IngredientIcon)
	{
		IngredientIcon->SetBrushFromTexture(ItemData->ItemIcon);
	}

	if (IngredientCountText)
	{
		IngredientCountText->SetText(FText::AsNumber(RequiredQuantity));
	}
}