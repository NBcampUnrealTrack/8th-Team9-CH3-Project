#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/ItemDataAsset.h"
#include "ItemResultEntryWidget.generated.h"


UCLASS()
class IAMLEGEND_API UItemResultEntryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void InitEntry(UItemDataAsset* ItemDataAsset, int32 Quantity);
	
	UPROPERTY(meta=(BindWidget))
	class UImage* Img_ItemIcon;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Text_ItemName;
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Text_ItemQuantity;
};
