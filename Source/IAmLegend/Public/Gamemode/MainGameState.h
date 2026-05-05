#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MainGameState.generated.h"


UCLASS()
class IAMLEGEND_API AMainGameState : public AGameState
{
	GENERATED_BODY()

public:
	AMainGameState();
	
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Score")
	int32 KillScore;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Stage")
	int32 CurrentStage;
	
private:
	void StartGame();
	void EnterStage();
	
};
