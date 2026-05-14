#include "MainHUD.h"

#include "TitleUIWidget.h"
#include "Blueprint/UserWidget.h"
#include "GameMode/MainGameInstance.h"
#include "UI/StageHUDWidget.h"
#include "UI/PauseMenuWidget.h"
#include "PlayerHealthWidget.h"
#include "Kismet/GameplayStatics.h" // [추가_민성] 현재 레벨 이름을 가져오기 위해 임시로 추가

void AMainHUD::BeginPlay()
{
	Super::BeginPlay();
	/*
	UMainGameInstance* GI = Cast<UMainGameInstance>(GetGameInstance());
	if (GI && !GI->GetbIsGameStarted())
	{
		ShowTitleHUD();
	}*/
	// 현재 열려있는 맵의 이름을 가져오기
	FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this);

	// 맵 이름이 "MainTitleLevel"일 때만 타이틀 UI 출력
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
		// MainTitleLevel이 아닌 다른 레벨에서 체력 UI 출력
		ShowPlayerHealthHUD();

		// 마우스 커서를 끄고 게임 입력 모드로 전환
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
void AMainHUD::HideStageHUD()
{
	if (StageHUDWidget)
	{
		StageHUDWidget->RemoveFromParent();
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

// 일시정지 메뉴 로직
void AMainHUD::TogglePauseMenu()
{
	// 타이틀 UI 예외 처리
	if (UGameplayStatics::GetCurrentLevelName(this) == "MainTitleLevel") return;

	APlayerController* PC = GetOwningPlayerController();
	if (!PC) return;

	// 위젯 생성
	if (!PauseMenuWidget && PauseMenuClass)
	{
		PauseMenuWidget = CreateWidget<UPauseMenuWidget>(GetWorld(), PauseMenuClass);
	}

	if (PauseMenuWidget)
	{
		if (PauseMenuWidget->IsInViewport())
		{
			// 해제 로직
			PauseMenuWidget->RemoveFromParent();
			UGameplayStatics::SetGamePaused(GetWorld(), false); // 시간 재개
			PC->bShowMouseCursor = false;
			PC->SetInputMode(FInputModeGameOnly());
		}
		else
		{
			// 실행 로직
			PauseMenuWidget->AddToViewport();
			UGameplayStatics::SetGamePaused(GetWorld(), true); // 시간 정지 (타이머 자동 정지)
			PC->bShowMouseCursor = true;
			PC->SetInputMode(FInputModeGameAndUI());
		}
	}
}