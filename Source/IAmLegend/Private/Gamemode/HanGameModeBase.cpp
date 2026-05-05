#include "Gamemode/HanGameModeBase.h"
#include "Character/HanPlayerCharacter.h"

AHanGameModeBase::AHanGameModeBase()
{
    DefaultPawnClass = AHanPlayerCharacter::StaticClass();
}