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
	void InitIngredient(UItemDataAsset* ItemData, int32 RequiredQuantity);

protected:
	UPROPERTY(meta = (BindWidget))
	UImage* IngredientIcon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* IngredientCountText;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* IngredientName;
};