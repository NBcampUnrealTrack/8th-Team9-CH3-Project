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
	    //필요한 재료의 개수 표시
		IngredientCountText->SetText(FText::AsNumber(RequiredQuantity));
	}
	
	if (IngredientName)
	{
		IngredientName->SetText(FText::FromString(ItemData->ItemName));
	}
}