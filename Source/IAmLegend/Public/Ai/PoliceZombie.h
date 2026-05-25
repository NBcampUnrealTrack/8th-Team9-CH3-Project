#pragma once

#include "CoreMinimal.h"
#include "Ai/Base_Zombie.h"
#include "PoliceZombie.generated.h"



UCLASS()
class IAMLEGEND_API APoliceZombie : public ABase_Zombie
{
    GENERATED_BODY()

public:
    APoliceZombie();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void Die() override;

};