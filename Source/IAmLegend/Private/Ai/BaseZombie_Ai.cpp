#include "Ai/BaseZombie_Ai.h"
#include "Character/HanPlayerCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AISenseConfig_Sight.h"

ABaseZombie_Ai::ABaseZombie_Ai()
{
    PrimaryActorTick.bCanEverTick = true;

    AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComp"));
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

    SightConfig->SightRadius = 1500.0f;
    SightConfig->LoseSightRadius = 2000.0f;
    SightConfig->PeripheralVisionAngleDegrees = 60.0f;
    SightConfig->SetMaxAge(5.0f);

    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

    AIPerceptionComp->ConfigureSense(*SightConfig);
    AIPerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());
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
            BlackboardComp->SetValueAsObject(TEXT("TargetActor"), Actor);
        }
        else if (PlayerChar->bIsStealth)
        {
            BlackboardComp->SetValueAsObject(TEXT("TargetActor"), nullptr);
        }
    }
}

void ABaseZombie_Ai::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!BlackboardComp) return;

    AHanPlayerCharacter* Player = Cast<AHanPlayerCharacter>(
        UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (!Player) return;

    AActor* CurrentTarget = Cast<AActor>(
        BlackboardComp->GetValueAsObject(TEXT("TargetActor")));

    if (!Player->bIsStealth && !CurrentTarget)
    {
        BlackboardComp->SetValueAsObject(TEXT("TargetActor"), Player);
    }
}