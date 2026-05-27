#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Item/InventoryComponent.h"
#include "MainGameStateBase.generated.h"

class UItemDataAsset;

UCLASS()
class IAMLEGEND_API AMainGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
	
public:
	AMainGameStateBase();
	
	//플레이어 킬 카운트 설정 및 전달
	int32 GetPlayerKillCount() const;
	void AddPlayerKillCount();

	
	// 이번 스테이지에서 획득한 아이템들을 모아둘 공용 장부 배열
	UPROPERTY(BlueprintReadOnly, Category = "StageResult")
	TArray<FItemSlot> StageAcquiredItems;

	// 인벤토리가 아이템을 주웠을 때 장부에 기록해달라고 요청할 함수
	void AddStageAcquiredItem(UItemDataAsset* ItemData, int32 Amount);

	// 새 스테이지 시작 시 장부를 깨끗하게 비워줄 초기화 함수
	void ClearStageAcquiredItems();
	
	//획득한 아이템 가져가기
	TArray<FItemSlot> GetAcquiredItems() const;
	
private:
	int32 KillCount;

	
};
