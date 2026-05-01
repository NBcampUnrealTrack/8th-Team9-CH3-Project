#include "MainGameMode.h"
#include "MainGameStateBase.h"

AMainGameMode::AMainGameMode()
{
	GameStateClass = AMainGameStateBase::StaticClass();
}
