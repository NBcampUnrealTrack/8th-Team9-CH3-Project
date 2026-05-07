#include "TestUserWidget.h"
#include "Gamemode/MainGameModeBase.h"
#include "Gamemode/MainGameInstance.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UTestUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (Btn_GameStart)
	{
		Btn_GameStart->OnClicked.AddDynamic(this, &UTestUserWidget::OnGameStartClicked);
	}
	
	
	UMainGameInstance* GI = Cast<UMainGameInstance>(GetGameInstance());
	if (GI && GI->GetGameStarted())
	{
		Btn_GameStart->SetVisibility(ESlateVisibility::Hidden);
		Txt_Shelter->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Btn_GameStart->SetVisibility(ESlateVisibility::Visible);
		Txt_Shelter->SetVisibility(ESlateVisibility::Hidden);
	}
	
	if (Btn_SelectHospitalStage && Btn_SelectPoliceStage && Btn_SelectSchoolStage)
	{
		Btn_SelectHospitalStage->SetVisibility(ESlateVisibility::Hidden);
		Btn_SelectPoliceStage->SetVisibility(ESlateVisibility::Hidden);
		Btn_SelectSchoolStage->SetVisibility(ESlateVisibility::Hidden);
		
		Btn_SelectHospitalStage->OnClicked.AddDynamic(this, &UTestUserWidget::OnHospitalStageClicked);
		Btn_SelectPoliceStage->OnClicked.AddDynamic(this, &UTestUserWidget::OnOnPoliceStageClicked);
		Btn_SelectSchoolStage->OnClicked.AddDynamic(this, &UTestUserWidget::OnSchoolStageClicked);
	}

}

void UTestUserWidget::OnGameStartClicked()
{
	AMainGameModeBase* GM = Cast<AMainGameModeBase>(GetWorld()->GetAuthGameMode());

	if (GM)
	{
		GM->StartGame();
	}
	
	Btn_GameStart->SetVisibility(ESlateVisibility::Hidden);
	Txt_Shelter->SetVisibility(ESlateVisibility::Visible);
}

void UTestUserWidget::OnHospitalStageClicked()
{
	AMainGameModeBase* GM = Cast<AMainGameModeBase>(GetWorld()->GetAuthGameMode());
	UMainGameInstance* GI = Cast<UMainGameInstance>(GetGameInstance());
	if (GM && GI)
	{
		GM->EnterStage(0);
		GI->SetUIPopUp(false);
	}
}

void UTestUserWidget::OnSchoolStageClicked()
{
	AMainGameModeBase* GM = Cast<AMainGameModeBase>(GetWorld()->GetAuthGameMode());
	UMainGameInstance* GI = Cast<UMainGameInstance>(GetGameInstance());
	if (GM && GI)
	{
		GM->EnterStage(1);
		GI->SetUIPopUp(false);
	}
}

void UTestUserWidget::OnOnPoliceStageClicked()
{
	AMainGameModeBase* GM = Cast<AMainGameModeBase>(GetWorld()->GetAuthGameMode());
	UMainGameInstance* GI = Cast<UMainGameInstance>(GetGameInstance());
	if (GM && GI)
	{
		GM->EnterStage(2);
		GI->SetUIPopUp(false);
	}
}
