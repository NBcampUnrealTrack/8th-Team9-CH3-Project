#include "Ai/PoliceZombie.h"


APoliceZombie::APoliceZombie()
{
    AttackCooldown = 4.0f;
    Health = 100.0f;
    AttackRange = 150.0f;
}

void APoliceZombie::BeginPlay()
{
    Super::BeginPlay();

}


void APoliceZombie::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}
