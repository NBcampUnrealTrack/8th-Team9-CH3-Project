#include "UI/StageHUDWidget.h"

#include "Components/TextBlock.h"
#include "Gamemode/MainGameModeBase.h"
#include "Gamemode/MainGameInstance.h"
#include "Gamemode/MainGameStateBase.h"
#include "Kismet/GameplayStatics.h"


void UStageHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	CachedGM = Cast<AMainGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	
	GetWorld()->GetTimerManager().SetTimer(UpdateTimerHandle, this, &UStageHUDWidget::UpdateStageHUD, 0.1f, true);
	
	UpdateKillCount();
	
	if (Text_CurrentStage)
	{
		//enum타입의 DisplayName 가져오기
		const UEnum* EnumPtr = StaticEnum<EStageType>();
		UMainGameInstance* GI = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		if (!GI) return;
		FText Text = EnumPtr->GetDisplayValueAsText(GI->GetCurrentStage());
		Text_CurrentStage->SetText(FText::FromString(FString::Printf(TEXT("%s"), *Text.ToString())));
	}
	
}

void UStageHUDWidget::UpdateStageHUD()
{
	if (!CachedGM) return;
	
	CurrentRemainingTime = CachedGM->GetRemainingStageTime();
	
	if (Text_RemainingTime)
	{
		Text_RemainingTime->SetText(FText::FromString(ConvertToClockTime(CurrentRemainingTime)));
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

//시간 포맷으로 변환
FString UStageHUDWidget::ConvertToClockTime(float RemainingSeconds) const
{
	if (!CachedGM) return TEXT("--:--");
	
	float MaxDuration = CachedGM->GetMaxStageDuration();
	float ElapsedSeconds = MaxDuration - FMath::Max(RemainingSeconds, 0.0f);
	// 디버그 로그
	UE_LOG(LogTemp, Warning, TEXT("MaxDuration: %.1f | Remaining: %.1f | Elapsed: %.1f"), 
		MaxDuration, RemainingSeconds, ElapsedSeconds);
	
	// 300초(5분) 동안 120분 경과 → 1초 = 24분
	const float GameMinutesPerRealSecond = 120.0f / 300.0f;
	float ElapsedGameMinutes = ElapsedSeconds * GameMinutesPerRealSecond;
	
	// 18:00 = 1080분 기준
	const int32 StartMinutes = 18 * 60;
	int32 TotalMinutes = StartMinutes + FMath::FloorToInt(ElapsedGameMinutes);

	int32 Hours   = (TotalMinutes / 60) % 24;
	int32 Minutes = TotalMinutes % 60;
	
	return FString::Printf(TEXT("%02d:%02d"), Hours, Minutes);
}
