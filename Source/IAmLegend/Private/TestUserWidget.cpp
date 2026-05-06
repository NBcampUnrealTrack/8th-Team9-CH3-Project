#include "TestUserWidget.h"
#include "Gamemode/MainGameStateBase.h"
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
	AMainGameStateBase* GameState = GetWorld()->GetGameState<AMainGameStateBase>();
	if (GameState)
	{
		GameState->StartGame();
	}
	
	Btn_GameStart->SetVisibility(ESlateVisibility::Hidden);
	Txt_Shelter->SetVisibility(ESlateVisibility::Visible);
}

void UTestUserWidget::OnHospitalStageClicked()
{
	AMainGameStateBase* GameState = GetWorld()->GetGameState<AMainGameStateBase>();
	if (GameState)
	{
		GameState->EnterStage(0);
	}
}

void UTestUserWidget::OnSchoolStageClicked()
{
	AMainGameStateBase* GameState = GetWorld()->GetGameState<AMainGameStateBase>();
	if (GameState)
	{
		GameState->EnterStage(1);
	}
}

void UTestUserWidget::OnOnPoliceStageClicked()
{
	AMainGameStateBase* GameState = GetWorld()->GetGameState<AMainGameStateBase>();
	if (GameState)
	{
		GameState->EnterStage(2);
	}
}
