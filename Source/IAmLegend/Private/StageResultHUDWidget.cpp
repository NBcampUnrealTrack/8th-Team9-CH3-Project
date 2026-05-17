#include "StageResultHUDWidget.h"
#include "GameMode/MainGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UStageResultHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (Btn_Return)
	{
		Btn_Return->OnClicked.AddDynamic(this, &UStageResultHUDWidget::OnReturnButtonClicked);
	}
	
	//스테이지 남은 시간 초기화
	UpdateStageRemainingTime();
	//죽인 좀비 수 초기화
	UpdateKillCount();
}

void UStageResultHUDWidget::OnReturnButtonClicked()
{
	UGameplayStatics::SetGamePaused(GetWorld(), false);
	
	UGameplayStatics::OpenLevel(GetWorld(), FName("Shelter"));
}

void UStageResultHUDWidget::UpdateStageRemainingTime()
{
	AMainGameModeBase* GM = Cast<AMainGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GM && Text_StageRemainingTime)
	{
		//float CurrentRemainingTime = GM->GetRemainingStageTime();
		//UE_LOG(LogTemp, Warning, TEXT("CurrentRemainingTime: %f"), CurrentRemainingTime);
		//FString String = FString::Printf(TEXT("스테이지 남은 시간 : %.1f"), FMath::Max(CurrentRemainingTime, 0.0f));
		//Text_StageRemainingTime->SetText(FText::FromString(String));
	}
		
}

void UStageResultHUDWidget::UpdateKillCount()
{
	AMainGameModeBase* GM = Cast<AMainGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GM && Text_KillCount)
	{
		//FString String = FString::Printf(TEXT("총 점수 : %d"), GM->GetPlayerKillCount());
		//Text_KillCount->SetText(FText::FromString(String));
	}
}
