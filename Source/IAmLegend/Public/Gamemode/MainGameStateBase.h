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
	
	//플레이어 킬 카운트 설정 및 전달
	int32 GetPlayerKillCount() const;
	void AddPlayerKillCount();

	
private:
	int32 KillCount;

	
};
