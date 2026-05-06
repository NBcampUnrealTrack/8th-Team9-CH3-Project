#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MainGameInstance.generated.h"


UCLASS()
class IAMLEGEND_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UMainGameInstance();
	
	void SetGameStarted(bool bStarted);
	bool GetGameStarted() const;
	
private:
	UPROPERTY(VisibleAnywhere, Category = "Game")
	bool bIsGameStarted;
};
