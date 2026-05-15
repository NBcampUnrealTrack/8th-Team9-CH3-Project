// PauseMenuWidget.cpp
#include "UI/PauseMenuWidget.h"
#include "GameMode/MainGameInstance.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "UI/MainHUD.h"

void UPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Resume 버튼 연결
	if (ResumeButton)
	{
		// 중복 에러 방지를 위해 RemoveDynamic 추가
		ResumeButton->OnClicked.RemoveDynamic(this, &UPauseMenuWidget::OnResumeClicked);
		ResumeButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnResumeClicked);
	}

	// Quit 버튼 연결
	if (QuitButton)
	{
		QuitButton->OnClicked.RemoveDynamic(this, &UPauseMenuWidget::OnQuitClicked);
		QuitButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnQuitClicked);
	}

	// Options 버튼 연결
	if (OptionsButton)
	{
		OptionsButton->OnClicked.RemoveDynamic(this, &UPauseMenuWidget::OnOptionsClicked);
		OptionsButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnOptionsClicked);
	}

	// Credit 버튼 연결
	if (CreditButton)
	{
		CreditButton->OnClicked.RemoveDynamic(this, &UPauseMenuWidget::OnCreditClicked);
		CreditButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnCreditClicked);
	}
}

void UPauseMenuWidget::OnResumeClicked()
{
	// HUD를 찾아 일시 정지 토글 함수를 다시 실행
	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		AMainHUD* HUD = Cast<AMainHUD>(PC->GetHUD());
		if (HUD)
		{
			HUD->TogglePauseMenu();
		}
	}
}

void UPauseMenuWidget::OnQuitClicked()
{
	// 일시 정지 해제
	UGameplayStatics::SetGamePaused(GetWorld(), false);

	// 게임 인스턴스 상태 초기화
	UMainGameInstance* GI = Cast<UMainGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->SetbIsGameStarted(false);
		GI->SetbIsStageStarted(false);
	}

	//  MaintitleLevel 맵으로 이동
	UGameplayStatics::OpenLevel(GetWorld(), FName("MainTitleLevel"));
}

void UPauseMenuWidget::OnOptionsClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		AMainHUD* HUD = Cast<AMainHUD>(PC->GetHUD());
		if (HUD)
		{
			HUD->ShowOptionHUD();
		}
	}
}

void UPauseMenuWidget::OnCreditClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		AMainHUD* HUD = Cast<AMainHUD>(PC->GetHUD());
		if (HUD)
		{
			HUD->ShowCreditHUD();
		}
	}
}