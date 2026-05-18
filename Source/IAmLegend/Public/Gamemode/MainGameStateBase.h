#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MainGameStateBase.generated.h"

UENUM()
enum class EStageType : uint8
{
	Shelter UMETA(DisplayName = "쉘터"),
	Hospital UMETA(DisplayName = "병원"),
	PoliceStation UMETA(DisplayName = "경찰서"),
	School UMETA(DisplayName = "학교"),
	Boss UMETA(DisplayName = "보스 스테이지")
};

UCLASS()
class IAMLEGEND_API AMainGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
	
public:
	AMainGameStateBase();
	
	//플레이어 킬 카운트 설정 및 전달
	int32 GetPlayerKillCount() const;
	void AddPlayerKillCount();
	
	//현재 스테이지 설정 및 전달
	void SetCurrentStage(EStageType StageType);
	EStageType GetCurrentStage() const;
	
private:
	int32 KillCount;
	EStageType CurrentStage;
	
};
