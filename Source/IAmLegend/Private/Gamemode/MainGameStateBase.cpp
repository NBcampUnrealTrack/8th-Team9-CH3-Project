#include "Gamemode/MainGameStateBase.h"


AMainGameStateBase::AMainGameStateBase()
{
	KillScore = 0;
	CurrentStage = 0;
}

void AMainGameStateBase::BeginPlay()
{
	Super::BeginPlay();
}
