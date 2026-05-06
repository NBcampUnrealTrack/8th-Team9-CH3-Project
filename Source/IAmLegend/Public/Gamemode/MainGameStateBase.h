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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> UIWidgetClass;
	
	//게임 시작
	void StartGame();
	
	//플레이어 스테이지 선택
	UFUNCTION(BlueprintCallable)
	void EnterStageSelectZone();
	//스테이지 진입
	void EnterStage(int32 StageIndex);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Level")
	TArray<FName> LevelMapNames;
private:


	
};
