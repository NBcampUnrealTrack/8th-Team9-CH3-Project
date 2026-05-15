// Fill out your copyright notice in the Description page of Project Settings.


#include "Ai/NurseZombie_ai.h"

#include "Ai/NurseZombie.h" // 추가

// NurseZombie_ai.cpp 내부
void ANurseZombie_ai::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
    Super::OnTargetDetected(Actor, Stimulus);

    ANurseZombie* Nurse = Cast<ANurseZombie>(GetPawn());
    if (!Nurse || !Actor || !Actor->ActorHasTag(TEXT("Player"))) return;

    if (Stimulus.WasSuccessfullySensed())
    {
        // 플레이어 발견 → Idle 상태일 때만 비명
        if (Nurse->GetCurrentState() == EZombieState::Idle)
        {
            Nurse->PlayScreamMontage();
        }
    }
    else
    {
        // 플레이어 시야 상실
        if (Nurse->GetCurrentState() == EZombieState::Screaming)
        {
            // 비명 중 놓침 → 플래그만 세워두고 몽타주가 끝나면 Idle로
            Nurse->bLostSightDuringScream = true;
        }
        else
        {
            // 추격 중 놓침 → Idle로 복귀
            Nurse->SetCurrentState(EZombieState::Idle);
        }
    }
}