#include "UI/ExtractionHUDWidget.h"
#include "UI/MainHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"


#include "EscapingPoint.h"
#include "Components/ProgressBar.h"

void UExtractionHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	//EscapingPoint 액터 가져오기
	AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), AEscapingPoint::StaticClass());
	EscapingPoint = Cast<AEscapingPoint>(FoundActor);
	if (!EscapingPoint) return;
	
	MaxEscapingDuration = EscapingPoint->GetMaxEscapingDuration();
	
	//UI 업데이트 타이머
	GetWorld()->GetTimerManager().SetTimer(UpdateTimerHandle, this, &UExtractionHUDWidget::UpdateExtractionHUD, 0.1f, true);
	
}

void UExtractionHUDWidget::UpdateExtractionHUD()
{
	if (!EscapingPoint) return;
	
	
	float CurrentRemainingTime = EscapingPoint->GetRemainingEscapingTime();
	float ClampedTime = FMath::Max(CurrentRemainingTime, 0.0f);
	
	if (Text_RemainingTime)
	{
		Text_RemainingTime->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), ClampedTime)));
	}
	
	if (PB_RemainingTime && MaxEscapingDuration > 0.0f)
	{
		float ElapsedTime = MaxEscapingDuration - ClampedTime;
		float Percent = ElapsedTime / MaxEscapingDuration;
		PB_RemainingTime->SetPercent(Percent);
	}
}

