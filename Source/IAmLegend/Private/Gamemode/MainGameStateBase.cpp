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
			if (UIWidget)
			{
				UIWidget->AddToViewport();
				PC->bShowMouseCursor = true;
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
	}
	
	UGameplayStatics::OpenLevel(GetWorld(), FName("Shelter"));
}
