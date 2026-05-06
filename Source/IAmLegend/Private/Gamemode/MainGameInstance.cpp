#include "Gamemode/MainGameInstance.h"

UMainGameInstance::UMainGameInstance()
{
	bIsGameStarted = false;
}

void UMainGameInstance::SetGameStarted(bool bStarted)
{
	bIsGameStarted = bStarted;
}

bool UMainGameInstance::GetGameStarted() const
{
	return bIsGameStarted;
}