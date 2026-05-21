// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/InventoryWidget.h"
#include "Item/InventorySlotWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/TextBlock.h" 
#include "Misc/DateTime.h"

void UInventoryWidget::RefreshInventory(const TArray<FItemSlot>& Inventory)
{
	InventoryGrid->ClearChildren();

	const int32 ColumnCount = 8;
	const int32 MaxSlotCount = 64;

	for (int32 i = 0; i < MaxSlotCount; i++)
	{
		UInventorySlotWidget* SlotWidget =
		   CreateWidget<UInventorySlotWidget>(
			  this,
			  SlotWidgetClass
		   );

		if (!SlotWidget) continue;

		
		SlotWidget->SlotIndex = i;

		// 실제 아이템 있는 슬롯만 데이터 적용
		if (Inventory.IsValidIndex(i))
		{
			SlotWidget->SetSlotData(Inventory[i]);
		}

		int32 Row = i / ColumnCount;
		int32 Col = i % ColumnCount;

		InventoryGrid->AddChildToUniformGrid(
		   SlotWidget,
		   Row,
		   Col
		);
	}
    UpdateDateTime();
}

void UInventoryWidget::UpdateDateTime()
{
    FDateTime CurrentTime = FDateTime::Now();
    
    FString DateString = CurrentTime.ToString(TEXT("%Y-%m-%d")); 
    
    EDayOfWeek DayOfWeek = CurrentTime.GetDayOfWeek();
    FString DayString = TEXT("");

    switch (DayOfWeek)
    {
    case EDayOfWeek::Monday:    DayString = TEXT("Monday"); break;
    case EDayOfWeek::Tuesday:   DayString = TEXT("Tuesday"); break;
    case EDayOfWeek::Wednesday: DayString = TEXT("Wednesday"); break;
    case EDayOfWeek::Thursday:  DayString = TEXT("Thursday"); break;
    case EDayOfWeek::Friday:    DayString = TEXT("Friday"); break;
    case EDayOfWeek::Saturday:  DayString = TEXT("Saturday"); break;
    case EDayOfWeek::Sunday:    DayString = TEXT("Sunday"); break;
    }
    

    DateString = FString::Printf(TEXT("%s %s"), *DateString, *DayString);
    FString TimeString = CurrentTime.ToString(TEXT("%H:%M"));

    if (Date)
    {
        Date->SetText(FText::FromString(DateString));
    }

    if (Time)
    {
        Time->SetText(FText::FromString(TimeString));
    }
}