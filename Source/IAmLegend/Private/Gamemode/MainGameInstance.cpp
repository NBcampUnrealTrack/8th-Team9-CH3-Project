#include "Gamemode/MainGameInstance.h"

UMainGameInstance::UMainGameInstance()
{
	bIsGameStarted = false;
	bIsUIPopUp = true;
}

void UMainGameInstance::SetGameStarted(bool bStarted)
{
	bIsGameStarted = bStarted;
}

bool UMainGameInstance::GetGameStarted() const
{
	return bIsGameStarted;
}

void UMainGameInstance::SetUIPopUp(bool bUIPopUp)
{
	bIsUIPopUp = bUIPopUp;
}

bool UMainGameInstance::GetUIPopUp() const
{
	return bIsUIPopUp;
}
