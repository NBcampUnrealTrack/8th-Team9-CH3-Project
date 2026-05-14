#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainGameModeBase.generated.h"

class UWidgetComponent;

UCLASS()
class IAMLEGEND_API AMainGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AMainGameModeBase();
	
	virtual void BeginPlay() override;
	
	
	//게임 시작
	void StartGame();
	//스테이지 진입
	void EnterStage(int32 StageIndex);
	//스테이지 종료
	void EndStage(bool bIsPlayerEscaped);
	//스테이지 남은 시간
	float GetRemainingStageTime() const;
	
	
	//적 처치
	void killedEnemy();
	int32 GetPlayerKillCount() const;

private:
	//스테이지 시작
	void StartStage();
	//스테이지 제한시간 종료
	void OnStageTimeUp();

	
	//타이머 핸들
	FTimerHandle StageTimer;
	
	//스테이지 제한시간
	UPROPERTY(EditAnywhere, Category="Stage")
	float MaxStageDuration;
	//맵 저장
	UPROPERTY(EditAnywhere, Category="Level")
	TArray<FName> LevelMapNames;
	//스테이지 UI
	UPROPERTY(VisibleAnywhere, Category = "UI")
	TObjectPtr<UWidgetComponent> StageWidget;
	//적 처치 횟수 (스테이지 정산 시스템에 사용 예정)
	int32 PlayerKillCount;
};
