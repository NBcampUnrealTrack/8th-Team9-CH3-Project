// StageChoiceWidget.cpp
// 요약: 스테이지 선택 UI 화면 출력

#include "StageChoiceWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UStageChoiceWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Hospital) Hospital->OnClicked.AddDynamic(this, &UStageChoiceWidget::OnHospitalClicked);
	if (School) School->OnClicked.AddDynamic(this, &UStageChoiceWidget::OnSchoolClicked);
	if (Police) Police->OnClicked.AddDynamic(this, &UStageChoiceWidget::OnPoliceClicked);
	if (Back) Back->OnClicked.AddDynamic(this, &UStageChoiceWidget::OnBackClicked);
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

	UGameplayStatics::OpenLevel(this, FName("HospitalStage"));
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

	UGameplayStatics::OpenLevel(this, FName("SchoolStage"));
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

	UGameplayStatics::OpenLevel(this, FName("PoliceStage"));
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