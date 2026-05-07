#include "Gamemode/MainGameStateBase.h"


AMainGameStateBase::AMainGameStateBase()
{
	KillScore = 0;
	CurrentStage = 0;
}

void AMainGameStateBase::BeginPlay()
{
	Super::BeginPlay();
}
/*
void AMainGameStateBase::StartGame()
{
	UMainGameInstance* GI = Cast<UMainGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->SetGameStarted(true);
		GI->SetUIPopUp(false);
	}
	
	UGameplayStatics::OpenLevel(GetWorld(), FName("Shelter"));
}

void AMainGameStateBase::EnterStageSelectZone()
{
	UMainGameInstance* GI = Cast<UMainGameInstance>(GetGameInstance());
	APlayerController* PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
	if (GI && PC)
	{
		GI->SetUIPopUp(true);
		PC->bShowMouseCursor = GI->GetUIPopUp();
	}
}

void AMainGameStateBase::EnterStage(int32 StageIndex)
{
	UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[StageIndex]);
	StartStage();
	
}

void AMainGameStateBase::StartStage()
{
	GetWorldTimerManager().SetTimer(StageTimerHandle, this, &AMainGameStateBase::OnStageTimeUp, StageDuration, false);
	UE_LOG(LogTemp, Warning, TEXT("Timer Start"));
}

void AMainGameStateBase::OnStageTimeUp()
{
	EndStage();
	UE_LOG(LogTemp, Warning, TEXT("Stage End"));
}

void AMainGameStateBase::EndStage()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName("Shelter"));
}
*/