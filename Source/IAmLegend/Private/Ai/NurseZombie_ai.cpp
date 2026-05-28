#include "Ai/NurseZombie_ai.h"
#include "Ai/NurseZombie.h"
#include "Character/HanPlayerCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AISenseConfig_Sight.h"

ANurseZombie_ai::ANurseZombie_ai()
{
    // 시야각 60도 설정 - 부모 생성자 이후 덮어씁니다
    if (SightConfig)
    {
        SightConfig->PeripheralVisionAngleDegrees = 60.0f; // 초기는 60
    }
}

void ANurseZombie_ai::ExpandSightAfterDetection()
{
    if (!SightConfig || !AIPerceptionComp) return;

    // ✅ 시야각을 360도로 확장
    SightConfig->PeripheralVisionAngleDegrees = 179.9f;

    // ✅ 변경사항 적용 (이 호출이 없으면 런타임에 반영 안 됨)
    AIPerceptionComp->RequestStimuliListenerUpdate();

    UE_LOG(LogTemp, Warning, TEXT("[Nurse] 시야 360도로 확장됨"));
}

void ANurseZombie_ai::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
    ANurseZombie* Nurse = Cast<ANurseZombie>(GetPawn());
    if (!Nurse || !Actor) return;

    if (Nurse->GetCurrentState() == EZombieState::Dead) return;

    AHanPlayerCharacter* Player = Cast<AHanPlayerCharacter>(Actor);
    if (!Player) return;

    if (Stimulus.WasSuccessfullySensed() && !Player->bIsStealth)
    {
        // ✅ 발견 즉시 시야 확장
        ExpandSightAfterDetection();

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
        else if (Nurse->GetCurrentState() != EZombieState::Attacking &&
            Nurse->GetCurrentState() != EZombieState::Hit)
        {
            if (BlackboardComp)
                BlackboardComp->SetValueAsObject(TEXT("TargetActor"), nullptr);
            Nurse->SetCurrentState(EZombieState::Idle);

            // ✅ 시야 잃으면 원래대로 복구 (선택사항)
            // SightConfig->PeripheralVisionAngleDegrees = 60.0f;
            // AIPerceptionComp->RequestStimuliListenerUpdate();
        }
    }
}