#include "MainGameMode.h"
#include "MainGameState.h"

AMainGameMode::AMainGameMode()
{
	GameStateClass = AMainGameState::StaticClass();
}
