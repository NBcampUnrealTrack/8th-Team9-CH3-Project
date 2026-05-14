#include "Gamemode/MainGameInstance.h"

UMainGameInstance::UMainGameInstance()
{
	bIsGameStarted = false;
	bIsStageStarted = false;
	bIsPlayerEscaped = false;
}

void UMainGameInstance::SetbIsGameStarted(bool bStarted)
{
	bIsGameStarted = bStarted;
}

bool UMainGameInstance::GetbIsGameStarted() const
{
	return bIsGameStarted;
}


void UMainGameInstance::SetbIsStageStarted(bool bStarted)
{
	bIsStageStarted = bStarted;
}

bool UMainGameInstance::GetbIsStageStarted() const
{
	return bIsStageStarted;
}

void UMainGameInstance::SetbIsPlayerEscaped(bool bEscaped)
{
	bIsPlayerEscaped = bEscaped;
}

bool UMainGameInstance::GetbIsPlayerEscaped() const
{
	return bIsPlayerEscaped;
}
