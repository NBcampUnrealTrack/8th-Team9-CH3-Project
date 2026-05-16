#include "Ai/NurseZombie_ai.h"
#include "Ai/NurseZombie.h"
#include "Character/HanPlayerCharacter.h"

void ANurseZombie_ai::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
    Super::OnTargetDetected(Actor, Stimulus);

    ANurseZombie* Nurse = Cast<ANurseZombie>(GetPawn());
    if (!Nurse || !Actor) return;

    // 태그 대신 캐스팅으로 플레이어 확인
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