#include "UI/MainHUD.h"


#include "TitleUIWidget.h"
#include "Blueprint/UserWidget.h"
#include "GameMode/MainGameInstance.h"
#include "UI/StageHUDWidget.h"
#include "UI/PauseMenuWidget.h"
#include "PlayerHealthWidget.h"
#include "Kismet/GameplayStatics.h"

void AMainHUD::BeginPlay()
{
	Super::BeginPlay();

	FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this);

	if (CurrentLevelName == "MainTitleLevel")
	{
		UMainGameInstance* GI = Cast<UMainGameInstance>(GetGameInstance());

		if (GI && !GI->GetbIsGameStarted())
		{
			ShowTitleHUD();
		}
	}
	else
	{
		ShowPlayerHealthHUD();

		APlayerController* PC = GetOwningPlayerController();

		if (PC)
		{
			PC->bShowMouseCursor = false;
			FInputModeGameOnly InputMode;
			PC->SetInputMode(InputMode);
		}
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

void AMainHUD::ShowPlayerHealthHUD()
{
	if (PlayerHealthClass && !PlayerHealthWidget)
	{
		PlayerHealthWidget = CreateWidget<UPlayerHealthWidget>(GetWorld(), PlayerHealthClass);
		if (PlayerHealthWidget)
		{
			PlayerHealthWidget->AddToViewport();
		}
	}
}

void AMainHUD::ShowExtractionHUD()
{
	if (ExtractionHUDClass)
	{
		ExtractionHUDWidget = CreateWidget<UUserWidget>(GetWorld(), ExtractionHUDClass);
		if (ExtractionHUDWidget)
		{
			ExtractionHUDWidget->AddToViewport();
		}
	}
}

void AMainHUD::HideExtractionHUD()
{
}

void AMainHUD::TogglePauseMenu()
{
	if (UGameplayStatics::GetCurrentLevelName(this) == "MainTitleLevel") return;

	APlayerController* PC = GetOwningPlayerController();
	if (!PC) return;

	if (!PauseMenuWidget && PauseMenuClass)
	{
		PauseMenuWidget = CreateWidget<UPauseMenuWidget>(GetWorld(), PauseMenuClass);
	}

	if (PauseMenuWidget)
	{
		if (PauseMenuWidget->IsInViewport())
		{
			PauseMenuWidget->RemoveFromParent();
			UGameplayStatics::SetGamePaused(GetWorld(), false);
			PC->bShowMouseCursor = false;
			PC->SetInputMode(FInputModeGameOnly());
		}
		else
		{
			PauseMenuWidget->AddToViewport();
			UGameplayStatics::SetGamePaused(GetWorld(), true);
			PC->bShowMouseCursor = true;
			PC->SetInputMode(FInputModeGameAndUI());
		}
	}
}
