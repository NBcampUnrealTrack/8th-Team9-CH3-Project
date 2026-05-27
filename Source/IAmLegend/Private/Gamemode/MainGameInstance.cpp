#include "Gamemode/MainGameInstance.h"

UMainGameInstance::UMainGameInstance()
{
	bIsGameStarted = false;
	bIsStageStarted = false;
	bIsPlayerEscaped = false;
	CurrentStage = EStageType::Shelter;
}

void UMainGameInstance::SetbIsGameStarted(bool bStarted)
{
	bIsGameStarted = bStarted;
}

bool UMainGameInstance::GetbIsGameStarted() const
{
	return bIsGameStarted;
}


void UMainGameInstance::SetbIsStageStarted(bool bStarted)
{
	bIsStageStarted = bStarted;
}

bool UMainGameInstance::GetbIsStageStarted() const
{
	return bIsStageStarted;
}

void UMainGameInstance::SetbIsPlayerEscaped(bool bEscaped)
{
	bIsPlayerEscaped = bEscaped;
}

bool UMainGameInstance::GetbIsPlayerEscaped() const
{
	return bIsPlayerEscaped;
}

//현재 스테이지 설정
void UMainGameInstance::SetCurrentStage(EStageType StageType)
{
	CurrentStage = StageType;
}
//현재 스테이지 반환
EStageType UMainGameInstance::GetCurrentStage() const
{
	return CurrentStage;
}

//인벤토리 아이템 추가
void UMainGameInstance::AddItemAtGlobalInventory(FItemSlot& ItemSlot)
{
	GlobalInventory.Add(ItemSlot);
}

// 인벤토리 아이템 가져가기
TArray<FItemSlot>& UMainGameInstance::GetItemsFromGlobalInventory()
{
	return GlobalInventory;
}