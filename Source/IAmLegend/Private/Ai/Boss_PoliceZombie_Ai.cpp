// Fill out your copyright notice in the Description page of Project Settings.


#include "Ai/Boss_PoliceZombie_Ai.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"

ABoss_PoliceZombie_Ai::ABoss_PoliceZombie_Ai()
{
    //  시야 상세 설정: 여기서 수치를 바꾸면 AI의 탐지 능력이 변합니다.
    if (SightConfig) {
        SightConfig->SightRadius = 5000.0f;           // 인식 거리 (50미터
        SightConfig->LoseSightRadius = 2000.0f;       // 놓치는 거리 (시야에서 멀어져도 20미터까지는 추격)
        SightConfig->PeripheralVisionAngleDegrees = 60.0f; // 시야각 (정면 기준 양옆 60도, 총 120도 범위를 봄)
        SightConfig->SetMaxAge(5.0f);                 // 마지막으로 본 위치를 5초간 기억함
    }
}
void ABoss_PoliceZombie_Ai::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
    if (!BlackboardComp || !Actor) return;
    if (Actor->ActorHasTag(TEXT("Player")))
    {
        // 처음 인식했을 때만 설정, 시야에서 사라져도 nullptr로 안 바꿈
        if (Stimulus.WasSuccessfullySensed())
        {
            BlackboardComp->SetValueAsObject(TEXT("TargetActor"), Actor);
        }
        // else 제거 → 시야에서 사라져도 계속 추격
    }
}