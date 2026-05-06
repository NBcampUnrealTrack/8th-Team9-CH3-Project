#include "Gamemode/MainGameStateBase.h"
#include "TestUserWidget.h"
#include "Gamemode/MainGameInstance.h"
#include "Kismet/GameplayStatics.h"

AMainGameStateBase::AMainGameStateBase()
{
	KillScore = 0;
	CurrentStage = 0;
}

void AMainGameStateBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (UIWidgetClass)
	{
		APlayerController* PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
		if (PC)
		{
			UUserWidget* UIWidget = CreateWidget<UUserWidget>(GetWorld(), UIWidgetClass);
			UMainGameInstance* GI = Cast<UMainGameInstance>(GetGameInstance());
			if (UIWidget && GI)
			{
				UIWidget->AddToViewport();
				PC->bShowMouseCursor = GI->GetUIPopUp();
			}
		}
	}
		
	
}

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
	
}
