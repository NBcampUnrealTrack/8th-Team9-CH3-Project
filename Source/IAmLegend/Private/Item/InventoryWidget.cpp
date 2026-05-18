// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/InventoryWidget.h"
#include "Item/InventorySlotWidget.h"
#include "Components/UniformGridPanel.h"

void UInventoryWidget::RefreshInventory(const TArray<FItemSlot>& Inventory)
{
	InventoryGrid->ClearChildren();

	const int32 ColumnCount = 5;
	const int32 MaxSlotCount = 25;

	for (int32 i = 0; i < MaxSlotCount; i++)
	{
		UInventorySlotWidget* SlotWidget =
			CreateWidget<UInventorySlotWidget>(
				this,
				SlotWidgetClass
			);

		if (!SlotWidget) continue;

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
}