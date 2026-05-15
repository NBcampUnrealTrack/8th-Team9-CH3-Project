// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Item/ItemDataAsset.h"
#include "CraftRecipe.generated.h"


USTRUCT(BlueprintType)
struct FCraftingIngredient
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UItemDataAsset* ItemData; // 필요한 아이템

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Quantity;           // 필요한 수량
};

/**
 * 
 */
UCLASS()
class IAMLEGEND_API UCraftRecipe : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// 조합에 필요한 재료들
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe")
	TArray<FCraftingIngredient> Ingredients;

	// 결과물
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe")
	UItemDataAsset* ResultItem;

	// 결과물 수량 (예: 탈출용 백신 10개)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Recipe")
	int32 ResultQuantity = 1;
	
};
