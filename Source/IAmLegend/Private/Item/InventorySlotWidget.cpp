#include "Item/InventorySlotWidget.h"
#include "Item/UseItemDataAsset.h"
#include "Components/Image.h"
#include "Item/ItemActionPopupWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Blueprint/SlateBlueprintLibrary.h"

static TWeakObjectPtr<UItemActionPopUpWidget> GlobalActivePopup = nullptr;
static TWeakObjectPtr<UItemActionPopUpWidget> GlobalActiveHoverName = nullptr;

void UInventorySlotWidget::SetSlotData(const FItemSlot& SlotData)
{
    CurrentSlotData = SlotData;
    if (SlotData.ItemData)
    {
       ItemIcon->SetVisibility(ESlateVisibility::Visible);
       Quantity->SetVisibility(ESlateVisibility::HitTestInvisible);
       
       if (SlotButton) 
       {
          SlotButton->SetVisibility(ESlateVisibility::HitTestInvisible);
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

    if (CurrentSlotData.ItemData && PopupWidgetClass)
    {
       UItemActionPopUpWidget* PopupWidget = CreateWidget<UItemActionPopUpWidget>(GetWorld(), PopupWidgetClass);
       if (PopupWidget)
       {
          PopupWidget->AddToViewport();
          PopupWidget->SetupPopup(CurrentSlotData, SlotIndex);
          
          GlobalActivePopup = PopupWidget;

          FVector2D MousePosition;
          if (GetOwningPlayer()->GetMousePosition(MousePosition.X, MousePosition.Y))
          {
             PopupWidget->SetPositionInViewport(MousePosition);
          }
       }
    }
}

void UInventorySlotWidget::NativeOnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    Super::NativeOnMouseEnter(MyGeometry, MouseEvent);

    if (!CurrentSlotData.ItemData || !PopupWidgetClass) return;

    if (GlobalActiveHoverName.IsValid() && GlobalActiveHoverName->IsInViewport())
    {
        GlobalActiveHoverName->RemoveFromParent();
        GlobalActiveHoverName = nullptr;
    }

    UItemActionPopUpWidget* HoverWidget = CreateWidget<UItemActionPopUpWidget>(GetWorld(), PopupWidgetClass);
    if (HoverWidget)
    {
        HoverWidget->AddToViewport();
        HoverWidget->ShowOnlyName(CurrentSlotData);
        
        GlobalActiveHoverName = HoverWidget;

        FVector2D MousePosition;
        if (GetOwningPlayer()->GetMousePosition(MousePosition.X, MousePosition.Y))
        {
            HoverWidget->SetPositionInViewport(MousePosition);
        }
    }
}

void UInventorySlotWidget::NativeOnMouseLeave(const FPointerEvent& MouseEvent)
{
    Super::NativeOnMouseLeave(MouseEvent);

    if (GlobalActiveHoverName.IsValid() && GlobalActiveHoverName->IsInViewport())
    {
        GlobalActiveHoverName->RemoveFromParent();
        GlobalActiveHoverName = nullptr;
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