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
	
	void StartGame();
	
private:

	void EnterStage();
	
};
