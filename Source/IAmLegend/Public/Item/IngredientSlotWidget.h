// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IngredientSlotWidget.generated.h"

class UImage;
class UTextBlock;
class UItemDataAsset;

UCLASS()
class IAMLEGEND_API UIngredientSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 재료 데이터를 받아 위젯 정보를 세팅하는 함수
	void InitIngredient(UItemDataAsset* ItemData, int32 RequiredQuantity);

protected:
	UPROPERTY(meta = (BindWidget))
	UImage* IngredientIcon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* IngredientCountText;
	
};
