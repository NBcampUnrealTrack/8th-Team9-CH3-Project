#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MainGameStateBase.generated.h"


UCLASS()
class IAMLEGEND_API AMainGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
	
public:
	AMainGameStateBase();
	
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Score")
	int32 KillScore;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Stage")
	int32 CurrentStage;
	
	//맵 저장
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Level")
	TArray<FName> LevelMapNames;
	
	/*
	//게임 시작
	void StartGame();
	
	//플레이어 스테이지 선택
	UFUNCTION(BlueprintCallable)
	void EnterStageSelectZone();
	//스테이지 진입
	void EnterStage(int32 StageIndex);
	//스테이지 시작
	void StartStage();
	//스테이지 제한시간 종료
	void OnStageTimeUp();
	//스테이지 종료
	void EndStage();
	
	//타이머 핸들
	FTimerHandle StageTimerHandle;
	
	*/


	
};
