// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/InventorySlotWidget.h"
#include "Item/UseItemDataAsset.h"
#include "Components/Image.h"
#include "Item/ItemActionPopupWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Blueprint/SlateBlueprintLibrary.h"

static TWeakObjectPtr<UItemActionPopUpWidget> GlobalActivePopup = nullptr;

void UInventorySlotWidget::SetSlotData(const FItemSlot& SlotData)
{
	CurrentSlotData = SlotData;
	if (SlotData.ItemData)
	{
		
		ItemIcon->SetVisibility(ESlateVisibility::Visible);
		Quantity->SetVisibility(ESlateVisibility::HitTestInvisible);
		
		// [수정] SlotButton이 진짜 존재할 때만 안전하게 제어합니다.
		if (SlotButton) 
		{
			SlotButton->SetVisibility(ESlateVisibility::Visible);
			SlotButton->SetIsEnabled(true);
		}

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
	CurrentSlotData = FItemSlot();
	ItemIcon->SetBrushFromTexture(nullptr);
	Quantity->SetText(FText::GetEmpty());
	
	if (SlotButton) 
	{
		SlotButton->SetIsEnabled(false);
		SlotButton->SetVisibility(ESlateVisibility::Collapsed);
	}
    
	
	ItemIcon->SetVisibility(ESlateVisibility::Collapsed);
	Quantity->SetVisibility(ESlateVisibility::Collapsed);
}

void UInventorySlotWidget::OnSlotClicked()
{
	if (GEngine)
	{
		FString DebugMsg = FString::Printf(TEXT("슬롯 버튼 클릭됨! 아이템: %s"), 
		   CurrentSlotData.ItemData ? *CurrentSlotData.ItemData->GetName() : TEXT("없음"));
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, DebugMsg);
	}

	// 1. 이미 화면에 유효한 팝업이 켜져 있다면 토글 및 삭제 처리
	if (GlobalActivePopup.IsValid() && GlobalActivePopup->IsInViewport())
	{
		if (GlobalActivePopup->TargetSlotData.ItemData == CurrentSlotData.ItemData)
		{
			GlobalActivePopup->RemoveFromParent();
			GlobalActivePopup = nullptr;
			return; 
		}
        
		GlobalActivePopup->RemoveFromParent();
		GlobalActivePopup = nullptr;
	}

	// 2. 아이템 데이터가 있고, 설정된 팝업 클래스가 있다면 새로 생성
	if (CurrentSlotData.ItemData && PopupWidgetClass)
	{
		UItemActionPopUpWidget* PopupWidget = CreateWidget<UItemActionPopUpWidget>(GetWorld(), PopupWidgetClass);
		if (PopupWidget)
		{
			PopupWidget->TargetSlotData = CurrentSlotData;
          
			
			PopupWidget->TargetIndex = SlotIndex; 

			PopupWidget->AddToViewport();
			GlobalActivePopup = PopupWidget;

			// 마우스 커서 위치에 띄우기
			FVector2D MousePosition;
			if (GetOwningPlayer()->GetMousePosition(MousePosition.X, MousePosition.Y))
			{
				PopupWidget->SetPositionInViewport(MousePosition);
			}
		}
	}
}

void UInventorySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (SlotButton)
	{
		SlotButton->OnClicked.AddUniqueDynamic(this, &UInventorySlotWidget::OnSlotClicked);
	}
}

