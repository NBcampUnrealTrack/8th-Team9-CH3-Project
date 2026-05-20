// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/InventoryWidget.h"
#include "Item/InventorySlotWidget.h"
#include "Components/UniformGridPanel.h"

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

		// ⭐ [핵심 추가] 생성된 슬롯 위젯에게 "너는 i번째 칸이야"라고 명확히 번호를 매깁니다!
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
}