#include "UI/ItemResultEntryWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"


void UItemResultEntryWidget::InitEntry(UItemDataAsset* ItemDataAsset, int32 Quantity)
{
	if (!ItemDataAsset) return;
	
	if (Text_ItemName)
	{
		Text_ItemName->SetText(FText::FromString(ItemDataAsset->ItemName));
	}
	
	if (Text_ItemQuantity)
	{
		FString String = FString::Printf(TEXT("%d"), Quantity);
		Text_ItemQuantity->SetText(FText::FromString(String));
		UE_LOG(LogTemp, Warning, TEXT("개수: %d"), Quantity);
	}
	
	if (Img_ItemIcon && ItemDataAsset->ItemIcon)
	{
		Img_ItemIcon->SetBrushFromTexture(ItemDataAsset->ItemIcon);
	}
}
