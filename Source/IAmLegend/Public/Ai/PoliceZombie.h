#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Ai/Base_Zombie.h"
#include "PoliceZombie.generated.h"

// 좀비의 현재 상태를 나타내는 열거형입니다. 블루프린트에서도 볼 수 있습니다.


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


};