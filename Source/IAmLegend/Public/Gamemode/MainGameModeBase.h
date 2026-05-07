#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainGameModeBase.generated.h"

UCLASS()
class IAMLEGEND_API AMainGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AMainGameModeBase();
	
	virtual void BeginPlay() override;
	
	//스테이지 제한시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Stage")
	float MaxStageDuration;
	//맵 저장
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Level")
	TArray<FName> LevelMapNames;
	//UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> TitleUIWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> StageSelectUIWidgetClass;
	
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
	void EndStage(bool bIsPlayerEscaped);
	
	//UI 생성
	void SpawnTitleUI();
	void SpawnStageSelectUI();
	
	//타이머 핸들
	FTimerHandle StageTimerHandle;
	

};
