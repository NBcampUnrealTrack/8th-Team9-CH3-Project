#include "Gamemode/MainGameMode.h"
#include "Gamemode/MainGameState.h"
#include "Character/HanPlayerCharacter.h"

AMainGameMode::AMainGameMode()
{
	GameStateClass = AMainGameState::StaticClass();
	DefaultPawnClass = AHanPlayerCharacter::StaticClass();
}
