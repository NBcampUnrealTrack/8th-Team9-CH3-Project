#include "TestUserWidget.h"
#include "Gamemode/MainGameModeBase.h"
#include "Gamemode/MainGameInstance.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UTestUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	Txt_Shelter->SetVisibility(ESlateVisibility::Visible);


	
	if (Btn_SelectHospitalStage && Btn_SelectPoliceStage && Btn_SelectSchoolStage)
	{
		Btn_SelectHospitalStage->SetVisibility(ESlateVisibility::Visible);
		Btn_SelectPoliceStage->SetVisibility(ESlateVisibility::Visible);
		Btn_SelectSchoolStage->SetVisibility(ESlateVisibility::Visible);
		
		Btn_SelectHospitalStage->OnClicked.AddDynamic(this, &UTestUserWidget::OnHospitalStageClicked);
		Btn_SelectPoliceStage->OnClicked.AddDynamic(this, &UTestUserWidget::OnOnPoliceStageClicked);
		Btn_SelectSchoolStage->OnClicked.AddDynamic(this, &UTestUserWidget::OnSchoolStageClicked);
	}

}


void UTestUserWidget::OnHospitalStageClicked()
{
	AMainGameModeBase* GM = Cast<AMainGameModeBase>(GetWorld()->GetAuthGameMode());
	UMainGameInstance* GI = Cast<UMainGameInstance>(GetGameInstance());
	if (GM && GI)
	{
		GM->EnterStage(0);

	}
}

void UTestUserWidget::OnSchoolStageClicked()
{
	AMainGameModeBase* GM = Cast<AMainGameModeBase>(GetWorld()->GetAuthGameMode());
	UMainGameInstance* GI = Cast<UMainGameInstance>(GetGameInstance());
	if (GM && GI)
	{
		GM->EnterStage(1);

	}
}

void UTestUserWidget::OnOnPoliceStageClicked()
{
	AMainGameModeBase* GM = Cast<AMainGameModeBase>(GetWorld()->GetAuthGameMode());
	UMainGameInstance* GI = Cast<UMainGameInstance>(GetGameInstance());
	if (GM && GI)
	{
		GM->EnterStage(2);

	}
}
