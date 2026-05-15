#include "UI/MainHUD.h"


#include "TitleUIWidget.h"
#include "Blueprint/UserWidget.h"
#include "GameMode/MainGameInstance.h"
#include "UI/StageHUDWidget.h"
#include "UI/PauseMenuWidget.h"
#include "UI/GameOverWidget.h"
#include "UI/CrosshairWidget.h"
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
		ShowCrosshairHUD();

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
	if (!StageHUDClass) return;
	
	if (StageHUDWidget == nullptr)
	{
		StageHUDWidget = CreateWidget<UStageHUDWidget>(GetWorld(), StageHUDClass);
	}
	
	if (StageHUDWidget)
	{
		StageHUDWidget->AddToViewport();
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
	if (!ExtractionHUDClass) return;
	
	if (ExtractionHUDWidget == nullptr)
	{
		ExtractionHUDWidget = CreateWidget<UUserWidget>(GetWorld(), ExtractionHUDClass);
	}
	
	if (ExtractionHUDWidget)
	{
		ExtractionHUDWidget->AddToViewport();
	}
}

void AMainHUD::HideExtractionHUD()
{
	if (ExtractionHUDWidget)
	{
		ExtractionHUDWidget->RemoveFromParent();
	}
}

void AMainHUD::ShowStageResultHUD()
{
	if (!StageResultHUDClass) return;
	
	if (StageResultHUDWidget == nullptr)
	{
		StageResultHUDWidget = CreateWidget<UUserWidget>(GetWorld(), StageResultHUDClass);
	}
	
	if (StageResultHUDWidget)
	{
		StageResultHUDWidget->AddToViewport();
	}
}

void AMainHUD::HideStageResultHUD()
{
	if (StageResultHUDWidget)
	{
		StageResultHUDWidget->RemoveFromParent();
	}
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

// 크레딧 함수
void AMainHUD::ShowCreditHUD()
{
	if (CreditHUDClass)
	{
		UUserWidget* CreditWidget = CreateWidget<UUserWidget>(GetWorld(), CreditHUDClass);
		if (CreditWidget)
		{
			CreditWidget->AddToViewport(100);
		}
	}
}

// 옵션 함수
void AMainHUD::ShowOptionHUD()
{
	if (OptionHUDClass)
	{
		UUserWidget* OptionWidget = CreateWidget<UUserWidget>(GetWorld(), OptionHUDClass);
		if (OptionWidget)
		{
			OptionWidget->AddToViewport(100);
		}
	}
}

// 게임 오버 함수
void AMainHUD::ShowGameOverHUD()
{
	if (GameOverClass)
	{
		UUserWidget* GameOverWidget = CreateWidget<UUserWidget>(GetWorld(), GameOverClass);
		if (GameOverWidget)
		{
			GameOverWidget->AddToViewport();

			// 마우스 커서를 보이게 하고 게임 입력을 UI로 전환
			APlayerController* PC = GetOwningPlayerController();
			if (PC)
			{
				PC->bShowMouseCursor = true;
				PC->SetInputMode(FInputModeUIOnly());
				UGameplayStatics::SetGamePaused(GetWorld(), true); // 게임 일시 정지
			}
		}
	}
}

// 조준점 함수
void AMainHUD::ShowCrosshairHUD()
{
	if (CrosshairClass && !CrosshairWidget)
	{
		CrosshairWidget = CreateWidget<UUserWidget>(GetOwningPlayerController(), CrosshairClass);
		if (CrosshairWidget)
		{
			CrosshairWidget->AddToViewport();
			// 마우스 클릭을 방해하지 않도록 기본 상태 설정
			CrosshairWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}
}

// 다른 UI 나올 때 조준점 숨기기
void AMainHUD::SetCrosshairVisibilty(bool bVisible)
{
	if (CrosshairWidget)
	{
		if (bVisible)
		{
			// on
			CrosshairWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			// off
			CrosshairWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
