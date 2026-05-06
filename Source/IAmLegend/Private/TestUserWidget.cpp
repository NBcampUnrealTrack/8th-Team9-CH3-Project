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