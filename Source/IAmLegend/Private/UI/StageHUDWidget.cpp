#include "UI/StageHUDWidget.h"

#include "Components/TextBlock.h"
#include "Gamemode/MainGameModeBase.h"
#include "Gamemode/MainGameInstance.h"
#include "Gamemode/MainGameStateBase.h"
#include "Kismet/GameplayStatics.h"


void UStageHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	GetWorld()->GetTimerManager().SetTimer(UpdateTimerHandle, this, &UStageHUDWidget::UpdateStageHUD, 0.1f, true);
	
	UpdateKillCount();
	
	if (Text_CurrentStage)
	{
		//enum타입의 DisplayName 가져오기
		const UEnum* EnumPtr = StaticEnum<EStageType>();
		UMainGameInstance* GI = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		FText Text = EnumPtr->GetDisplayValueAsText(GI->GetCurrentStage());
		Text_CurrentStage->SetText(FText::FromString(FString::Printf(TEXT("%s"), *Text.ToString())));
	}
	
}

void UStageHUDWidget::UpdateStageHUD()
{
	AMainGameModeBase* GM = Cast<AMainGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	
	if (GM)
	{
		CurrentRemainingTime = GM->GetRemainingStageTime();
	}
	
	if (Text_RemainingTime)
	{
		FString String = FString::Printf(TEXT("Time: %.1f"), FMath::Max(CurrentRemainingTime, 0.0f));
		Text_RemainingTime->SetText(FText::FromString(String));
	}
}

void UStageHUDWidget::UpdateRemainingTime(float RemainingSeconds)
{
	CurrentRemainingTime = RemainingSeconds;
	
	if (Text_RemainingTime)
	{
		Text_RemainingTime->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), FMath::Max(CurrentRemainingTime, 0.0f))));
	}
}

void UStageHUDWidget::UpdateKillCount()
{
	AMainGameStateBase* GS = Cast<AMainGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
	
	if (GS)
	{
		Text_KillCount->SetText(FText::FromString(FString::FromInt(GS->GetPlayerKillCount())));
	}
}