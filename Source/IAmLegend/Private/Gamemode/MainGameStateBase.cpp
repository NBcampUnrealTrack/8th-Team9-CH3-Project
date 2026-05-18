#include "Gamemode/MainGameStateBase.h"


AMainGameStateBase::AMainGameStateBase()
{
	KillCount = 0;
}
//플레이어 킬 카운트 증가
void AMainGameStateBase::AddPlayerKillCount()
{
	++KillCount;
}
//플레이어 킬 카운트 반환
int32 AMainGameStateBase::GetPlayerKillCount() const
{
	return KillCount;
}
