#pragma once
#include "CoreMinimal.h"
#include "Ai/Base_Zombie.h"
#include "NurseZombie.generated.h"

UCLASS()
class IAMLEGEND_API ANurseZombie : public ABase_Zombie
{
    GENERATED_BODY()

public:
    ANurseZombie();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Animation")
    virtual void PlayScreamMontage();

    virtual void SetCurrentState(EZombieState NewState) override;

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
        class AController* EventInstigator, AActor* DamageCauser) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    class UAnimMontage* ScreamMontage;

    UFUNCTION()
    void OnScreamMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    bool bLostSightDuringScream = false;
};