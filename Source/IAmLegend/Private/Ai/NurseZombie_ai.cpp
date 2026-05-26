#include "Ai/NurseZombie_ai.h"
#include "Ai/NurseZombie.h"
#include "Character/HanPlayerCharacter.h"

void ANurseZombie_ai::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
    Super::OnTargetDetected(Actor, Stimulus);

    ANurseZombie* Nurse = Cast<ANurseZombie>(GetPawn());
    if (!Nurse || !Actor) return;

    // ✅ Dead 상태면 차단
    if (Nurse->GetCurrentState() == EZombieState::Dead) return;

    AHanPlayerCharacter* Player = Cast<AHanPlayerCharacter>(Actor);
    if (!Player) return;

    if (Stimulus.WasSuccessfullySensed())
    {
        if (Nurse->GetCurrentState() == EZombieState::Idle)
        {
            Nurse->PlayScreamMontage();
        }
    }
    else
    {
        if (Nurse->GetCurrentState() == EZombieState::Screaming)
        {
            Nurse->bLostSightDuringScream = true;
        }
        else
        {
            Nurse->SetCurrentState(EZombieState::Idle);
        }
    }
}