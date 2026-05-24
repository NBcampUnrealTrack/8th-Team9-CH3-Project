#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Item/InventoryComponent.h"
#include "MainGameInstance.generated.h"

UENUM()
enum class EStageType : uint8
{
	Shelter UMETA(DisplayName = "Shelter"),
	Hospital UMETA(DisplayName = "Hospital"),
	PoliceStation UMETA(DisplayName = "PoliceStation"),
	School UMETA(DisplayName = "School"),
	Boss UMETA(DisplayName = "Boss")
};

UCLASS()
class IAMLEGEND_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UMainGameInstance();
	
	//게임 시작 여부 설정 및 확인
	void SetbIsGameStarted(bool bStarted);
	bool GetbIsGameStarted() const;
	
	//UI 팝업 여부 설정 및 확인 / 마우스 커서 필요한가 아닌가
	void SetUIPopUp(bool bUIPopUp);
	bool GetUIPopUp() const;
	
	//스테이지 시작 여부 설정 및 확인
	void SetbIsStageStarted(bool bStarted);
	bool GetbIsStageStarted() const;
	
	//스테이지 탈출 여부 설정 및 확인
	void SetbIsPlayerEscaped(bool bEscaped);
	bool GetbIsPlayerEscaped() const;
	
	//현재 스테이지 설정 및 전달
	void SetCurrentStage(EStageType StageType);
	EStageType GetCurrentStage() const;

	//인벤토리 아이템 데이터 관리
	void AddItemAtGlobalInventory(FItemSlot& ItemSlot);
	TArray<FItemSlot>& GetItemsFromGlobalInventory();
	
private:
	
	//인벤토리 관련 데이터 아이템 데이터가 이제 여기로 들어옴
	// 실제 아이템 데이터는 여기에 저장됩니다.
	UPROPERTY()
	TArray<FItemSlot> GlobalInventory;
	
	//게임 시작 여부
	bool bIsGameStarted;
	//UI 팝업 여부
	bool bIsUIPopUp;
	//스테이지 시작 여부
	bool bIsStageStarted;
	//스테이지 탈출 여부
	bool bIsPlayerEscaped;
	//현재 스테이지 정보
	EStageType CurrentStage;
};
