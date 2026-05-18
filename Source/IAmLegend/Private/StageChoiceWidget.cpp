// StageChoiceWidget.cpp
// 요약: 스테이지 선택 UI 화면 출력

#include "StageChoiceWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "GameMode/MainGameModeBase.h"

void UStageChoiceWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Hospital) Hospital->OnClicked.AddDynamic(this, &UStageChoiceWidget::OnHospitalClicked);
	if (School) School->OnClicked.AddDynamic(this, &UStageChoiceWidget::OnSchoolClicked);
	if (Police) Police->OnClicked.AddDynamic(this, &UStageChoiceWidget::OnPoliceClicked);
	if (Back) Back->OnClicked.AddDynamic(this, &UStageChoiceWidget::OnBackClicked);
	if (Boss) Boss->OnClicked.AddDynamic(this, &UStageChoiceWidget::OnBossClicked);
}

void UStageChoiceWidget::OnHospitalClicked()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(false);
	}
	AMainGameModeBase* GameMode = Cast<AMainGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode) 
	{
		GameMode->EnterStage(0);
	}
	
}

void UStageChoiceWidget::OnSchoolClicked()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(false);
	}

	AMainGameModeBase* GameMode = Cast<AMainGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode) 
	{
		GameMode->EnterStage(1);
	}
}

void UStageChoiceWidget::OnPoliceClicked()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(false);
	}

	AMainGameModeBase* GameMode = Cast<AMainGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode) 
	{
		GameMode->EnterStage(2);
	}
}

void UStageChoiceWidget::OnBackClicked()
{
	RemoveFromParent();

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(false);
	}
}

void UStageChoiceWidget::OnBossClicked()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(false);
	}

	AMainGameModeBase* GameMode = Cast<AMainGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		GameMode->EnterStage(3);
	}
}