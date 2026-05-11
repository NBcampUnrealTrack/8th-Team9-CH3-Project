// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/InventoryWidget.h"
#include "Item/InventorySlotWidget.h"
#include "Components/UniformGridPanel.h"

void UInventoryWidget::RefreshInventory(const TArray<FItemSlot>& Inventory)
{
	InventoryGrid->ClearChildren();

	const int32 ColumnCount = 5;

	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		UInventorySlotWidget* SlotWidget =
			CreateWidget<UInventorySlotWidget>(
			this,
				SlotWidgetClass
			);

		if (!SlotWidget) continue;

		SlotWidget->SetSlotData(Inventory[i]);

		int32 Row = i / ColumnCount;
		int32 Col = i % ColumnCount;

		InventoryGrid->AddChildToUniformGrid(
			SlotWidget,
			Row,
			Col
		);
	}
}
