#include "Ai/PoliceZombie.h"


APoliceZombie::APoliceZombie()
{
    AttackCooldown = 4.0f; // 공격 쿨타임 현재 4초의 텀을 가지고 공격함
    Health = 100.0f; // 좀비 체력 현재 100
    AttackRange = 150.0f; // 좀비와 플레이어간의 거리 약 15CM거리에 들어서면 공격모션을 취합니다
}

void APoliceZombie::BeginPlay()
{
    Super::BeginPlay();

}


void APoliceZombie::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}
