#include "Gamemode/MainGameStateBase.h"


AMainGameStateBase::AMainGameStateBase()
{
	KillCount = 0;
	CurrentStage = EStageType::Shelter;
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
//현재 스테이지 설정
void AMainGameStateBase::SetCurrentStage(EStageType StageType)
{
	CurrentStage = StageType;
}
//현재 스테이지 반환
EStageType AMainGameStateBase::GetCurrentStage() const
{
	return CurrentStage;
}