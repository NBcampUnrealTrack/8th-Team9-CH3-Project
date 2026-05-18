#include "Gamemode/MainGameStateBase.h"
#include "Item/ItemDataAsset.h"
#include "Item/InventoryComponent.h"

AMainGameStateBase::AMainGameStateBase()
{
	KillCount = 0;
}
//플레이어 킬 카운트 증가
void AMainGameStateBase::AddPlayerKillCount()
{
	++KillCount;
}
//플레이어 킬 카운트 반환
int32 AMainGameStateBase::GetPlayerKillCount() const
{
	return KillCount;
}


void AMainGameStateBase::AddStageAcquiredItem(UItemDataAsset* ItemData, int32 Amount)
{
	if (!ItemData || Amount <= 0) return;

	// 이미 장부에 적힌 아이템인지 확인해서 개수만 누적
	for (FItemSlot& Slot : StageAcquiredItems)
	{
		if (Slot.ItemData == ItemData)
		{
			Slot.Quantity += Amount;
			return;
		}
	}

	// 처음 줍는 아이템이면 장부에 새로 등록
	StageAcquiredItems.Add(FItemSlot(ItemData, Amount));
}

void AMainGameStateBase::ClearStageAcquiredItems()
{
	// 다음 판을 위해 장부 깔끔하게 비우기
	StageAcquiredItems.Empty();
}