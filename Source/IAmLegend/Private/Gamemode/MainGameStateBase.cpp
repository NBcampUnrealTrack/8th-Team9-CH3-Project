#include "Gamemode/MainGameStateBase.h"
#include "TestUserWidget.h"

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
	
}
