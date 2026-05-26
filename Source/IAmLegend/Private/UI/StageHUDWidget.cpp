#include "UI/StageHUDWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Gamemode/MainGameModeBase.h"
#include "Gamemode/MainGameInstance.h"
#include "Gamemode/MainGameStateBase.h"
#include "Kismet/GameplayStatics.h"


void UStageHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	//여러번 사용할 GameMode 미리 캐싱 후 저장
	CachedGM = Cast<AMainGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	
	//점멸 관련 설정
	BlinkTime = 30.f;
	bIsBlinkVisible = true;
	bAlphaGoingDown = true;
	CurrentAlpha = 1.f;
	
	GetWorld()->GetTimerManager().SetTimer(UpdateTimerHandle, this, &UStageHUDWidget::UpdateStageHUD, 0.1f, true);
	
	
	if (Text_CurrentStage)
	{
		//enum타입의 DisplayName 가져오기
		const UEnum* EnumPtr = StaticEnum<EStageType>();
		UMainGameInstance* GI = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		if (!GI) return;
		FText Text = EnumPtr->GetDisplayValueAsText(GI->GetCurrentStage());
		Text_CurrentStage->SetText(FText::FromString(FString::Printf(TEXT("%s"), *Text.ToString())));
	}
	
	if (Img_SkullIcon)
	{
		Img_SkullIcon->SetVisibility(ESlateVisibility::Hidden);
	}
	
}

void UStageHUDWidget::UpdateStageHUD()
{
	if (!CachedGM) return;
	
	CurrentRemainingTime = CachedGM->GetRemainingStageTime();
	
	if (Text_RemainingTime)
	{
		Text_RemainingTime->SetText(FText::FromString(ConvertToClockTime(CurrentRemainingTime)));
		
		if (CurrentRemainingTime <= 0.0f)
		{
			StopBlinkEffect();
			Text_RemainingTime->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f)));
			FString String = FString::Printf(TEXT("--:--"));
			Text_RemainingTime->SetText(FText::FromString(String));
		}
		else if (CurrentRemainingTime <= BlinkTime)
		{
			bAlphaGoingDown = true;
			Text_RemainingTime->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
			if (!GetWorld()->GetTimerManager().IsTimerActive(BlinkTimerHandle))
			{
				StartBlinkEffect();
			}
			Img_SkullIcon->SetVisibility(ESlateVisibility::Visible);
		}
	}
	UpdateKillCount();
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

void UStageHUDWidget::StartBlinkEffect()
{
	bIsBlinkVisible = true;
	GetWorld()->GetTimerManager().SetTimer(BlinkTimerHandle, this, &UStageHUDWidget::OnBlink, 0.5f, true);
}

void UStageHUDWidget::StopBlinkEffect()
{
	GetWorld()->GetTimerManager().ClearTimer(BlinkTimerHandle);
}

void UStageHUDWidget::OnBlink()
{
	if (!Text_RemainingTime) return;
	
	
	if (bAlphaGoingDown)
	{
		CurrentAlpha -= 0.05f;
		if (CurrentAlpha <= 0.2f)
		{
			CurrentAlpha = 0.2f;
			bAlphaGoingDown = false;
		}
	}
	else
	{
		CurrentAlpha += 0.05f;
		if (CurrentAlpha >= 1.f)
		{
			CurrentAlpha = 1.f;
			bAlphaGoingDown = true;
		}
	}

	Text_RemainingTime->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.0f, 0.0f, CurrentAlpha)));

}
