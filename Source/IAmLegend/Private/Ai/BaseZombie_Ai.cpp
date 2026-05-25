// Fill out your copyright notice in the Description page of Project Settings.


#include "Ai/BaseZombie_Ai.h"
#include "Character/HanPlayerCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AISenseConfig_Sight.h"

ABaseZombie_Ai::ABaseZombie_Ai()
{
    // 1. 컴포넌트 생성: 메모리에 AI의 눈(Perception)과 시야설정(SightConfig) 자리를 만듭니다.
    AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComp"));
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

    // 2. 시야 상세 설정: 여기서 수치를 바꾸면 AI의 탐지 능력이 변합니다.
    SightConfig->SightRadius = 1500.0f;           // 인식 거리 (15미터
    SightConfig->LoseSightRadius = 2000.0f;       // 놓치는 거리 (시야에서 멀어져도 20미터까지는 추격)
    SightConfig->PeripheralVisionAngleDegrees = 60.0f; // 시야각 (정면 기준 양옆 60도, 총 120도 범위를 봄)
    SightConfig->SetMaxAge(5.0f);                 // 마지막으로 본 위치를 5초간 기억함

    // 3. 소속(Affiliation) 설정: 언리얼 시스템상 적/아군/중립 모두 감지하도록 켭니다.
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
    //SightConfig->bLoseSightOnObstruction = true;

    // 4. 이벤트 연결: '타겟을 인지했을 때' 실행할 함수를 등록합니다. (델리게이트 바인딩)
    AIPerceptionComp->ConfigureSense(*SightConfig);
    AIPerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());

    // 4. 이벤트 연결
    AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &ABaseZombie_Ai::OnTargetDetected);
}
void ABaseZombie_Ai::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (BBAsset && BTAsset)
    {
        if (UseBlackboard(BBAsset, BlackboardComp))
        {
            if (RunBehaviorTree(BTAsset))
            {
                APlayerController* PC = GetWorld()->GetFirstPlayerController();
                if (PC && PC->GetPawn())
                {
                    APawn* PlayerPawn = PC->GetPawn();

                    // [추가] 0번 플레이어라도 "Player" 태그가 있어야만 블랙보드에 등록
                    if (PlayerPawn->ActorHasTag(TEXT("Player")))
                    {
                        BlackboardComp->SetValueAsObject(TEXT("TargetActor"), PlayerPawn);
                    }
                }
            }
        }
    }
}
void ABaseZombie_Ai::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
    if (!BlackboardComp || !Actor) return;

    if (Actor->ActorHasTag(TEXT("Player")))
    {
        AHanPlayerCharacter* PlayerChar = Cast<AHanPlayerCharacter>(Actor);
        if (!PlayerChar) return;

        if (Stimulus.WasSuccessfullySensed() && !PlayerChar->bIsStealth)
        {
            // 플레이어 발견 → 타겟 설정
            BlackboardComp->SetValueAsObject(TEXT("TargetActor"), Actor);
        }
        else if (PlayerChar->bIsStealth)
        {
            // 은신 중 → 타겟 해제
            BlackboardComp->SetValueAsObject(TEXT("TargetActor"), nullptr);
        }
        // 은신 해제 후 시야에 다시 들어오면 자동으로 첫 번째 조건에서 타겟 재설정됨
    }
}
void ABaseZombie_Ai::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!BlackboardComp) return;

    AHanPlayerCharacter* Player = Cast<AHanPlayerCharacter>(
        UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (!Player) return;

    // 은신 해제 상태이고 현재 타겟이 없으면 즉시 재설정
    AActor* CurrentTarget = Cast<AActor>(
        BlackboardComp->GetValueAsObject(TEXT("TargetActor")));

    if (!Player->bIsStealth && !CurrentTarget)
    {
        BlackboardComp->SetValueAsObject(TEXT("TargetActor"), Player);
    }
}