#include "Gamemode/MainGameModeBase.h"
#include "Character/HanPlayerCharacter.h"
#include "Gamemode/MainGameStateBase.h"

AMainGameModeBase::AMainGameModeBase()
{
	GameStateClass = AMainGameStateBase::StaticClass();
	DefaultPawnClass = AHanPlayerCharacter::StaticClass();
}

