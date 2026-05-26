#include "Item/ItemActionPopUpWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h" 
#include "Item/UseItemDataAsset.h"
#include "Item/ItemDataAsset.h"
#include "WeaponDataAsset.h"  
#include "Character/HanPlayerCharacter.h"
#include "Item/InventoryComponent.h"
#include "BattleLogic/Attachment/AttachmentDataAsset.h"

void UItemActionPopUpWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UseButton)
	{
		UseButton->OnClicked.AddUniqueDynamic(this, &UItemActionPopUpWidget::OnUseButtonClicked);
	}
}

void UItemActionPopUpWidget::SetupPopup(const FItemSlot& InSlotData, int32 InIndex)
{
	TargetSlotData = InSlotData;
	TargetIndex = InIndex;

	if (!TargetSlotData.ItemData || !ButtonText || !UseButton)
	{
		RemoveFromParent(); 
		return;
	}

	if (UWeaponDataAsset* WeaponData = Cast<UWeaponDataAsset>(TargetSlotData.ItemData))
	{
		ButtonText->SetText(FText::FromString(TEXT("무기 장착")));
		SetVisibility(ESlateVisibility::Visible); 
		return;
	}

	if(UAttachmentDataAsset* AttachmentData = Cast<UAttachmentDataAsset>(TargetSlotData.ItemData))
	{
		ButtonText->SetText(FText::FromString(TEXT("부착물 장착")));
		SetVisibility(ESlateVisibility::Visible); 
		return;
	}

	if (UUseItemDataAsset* UseItemData = Cast<UUseItemDataAsset>(TargetSlotData.ItemData))
	{
		if (UseItemData->EffectType == EItemType::Buff)
		{
			ButtonText->SetText(FText::FromString(TEXT("아이템 사용")));
			SetVisibility(ESlateVisibility::Visible); 
			return;
		}
	}

	RemoveFromParent(); 
}

void UItemActionPopUpWidget::OnUseButtonClicked()
{
	if (TargetIndex != INDEX_NONE && TargetSlotData.ItemData)
	{
		APlayerController* PC = GetOwningPlayer();
		if (PC)
		{
			AHanPlayerCharacter* PlayerChar = Cast<AHanPlayerCharacter>(PC->GetPawn());
			if (PlayerChar)
			{
				UInventoryComponent* InvComp = Cast<UInventoryComponent>(PlayerChar->GetComponentByClass(UInventoryComponent::StaticClass()));
				if (InvComp)
				{
					InvComp->UseItem(TargetIndex);
					InvComp->ShowInventory();
				}
			}
		}
		RemoveFromParent();
	}
}
void UItemActionPopUpWidget::ShowOnlyName(const FItemSlot& InSlotData)
{
	TargetSlotData = InSlotData;

	if (ItemNameText)
	{
		
		ItemNameText->SetText(FText::FromString(TargetSlotData.ItemData->ItemName));
		ItemNameText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		
		if (Cast<UWeaponDataAsset>(TargetSlotData.ItemData))
		{
			
			FLinearColor WeaponColor = FLinearColor(0.8f, 0.0f, 0.0f, 1.0f); // R, G, B, A
			ItemNameText->SetColorAndOpacity(FSlateColor(WeaponColor));
		}
		else
		{
			ItemNameText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		}
	}

	if (UseButton)
	{
		UseButton->SetVisibility(ESlateVisibility::Collapsed);
	}
}