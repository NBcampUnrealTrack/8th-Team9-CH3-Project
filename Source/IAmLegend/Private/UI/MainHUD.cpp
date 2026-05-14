#include "UI/MainHUD.h"

#include "TitleUIWidget.h"
#include "Blueprint/UserWidget.h"
#include "GameMode/MainGameInstance.h"
#include "UI/StageHUDWidget.h"


void AMainHUD::BeginPlay()
{
	Super::BeginPlay();
	
	UMainGameInstance* GI = Cast<UMainGameInstance>(GetGameInstance());
	if (GI && !GI->GetbIsGameStarted())
	{
		ShowTitleHUD();
	}
}

void AMainHUD::ShowTitleHUD()
{
	if (TitleHUDClass)
	{
		TitleHUDWidget = CreateWidget<UTitleUIWidget>(GetWorld(), TitleHUDClass);
		if (TitleHUDWidget)
		{
			TitleHUDWidget->AddToViewport();
		}
	}
	
	APlayerController* PC = GetOwningPlayerController();
	if (PC)
	{
		PC->bShowMouseCursor = true;
		FInputModeUIOnly InputMode;
		PC->SetInputMode(InputMode);
	}
}

void AMainHUD::HideTitleHUD()
{
	TitleHUDWidget->RemoveFromViewport();
}

void AMainHUD::ShowStageHUD()
{
	if (StageHUDClass)
	{
		StageHUDWidget = CreateWidget<UStageHUDWidget>(GetWorld(), StageHUDClass);
		if (StageHUDWidget)
		{
			StageHUDWidget->AddToViewport();
		}
	}
}
void AMainHUD::HideStageHUD()
{
	if (StageHUDWidget)
	{
		StageHUDWidget->RemoveFromParent();
	}
}