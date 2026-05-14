// PauseMenuWidget.cpp

#include "UI/PauseMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "MainHUD.h"

void UPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ResumeButton)
	{
		// 중복 에러 방지를 위해 RemoveDynamic 추가
		ResumeButton->OnClicked.RemoveDynamic(this, &UPauseMenuWidget::OnResumeClicked);
		ResumeButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnResumeClicked);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked.RemoveDynamic(this, &UPauseMenuWidget::OnQuitClicked);
		QuitButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnQuitClicked);
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
	// 타이틀 맵으로 돌아가거나 게임 종료
	UGameplayStatics::OpenLevel(GetWorld(), FName("MainTitleLevel"));
}