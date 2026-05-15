#include "UI/ExtractionHUDWidget.h"
#include "UI/MainHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"


#include "EscapingPoint.h"

void UExtractionHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	//UI 업데이트 타이머
	GetWorld()->GetTimerManager().SetTimer(UpdateTimerHandle, this, &UExtractionHUDWidget::UpdateExtractionHUD, 0.1f, true);
	
}

void UExtractionHUDWidget::UpdateExtractionHUD()
{
	AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), AEscapingPoint::StaticClass());
	AEscapingPoint* EscapingPoint = Cast<AEscapingPoint>(FoundActor);
;	float CurrentRemainingTime = EscapingPoint->GetRemainingEscapingTime();
	
	if (Text_RemainingTime)
	{
		Text_RemainingTime->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), FMath::Max(CurrentRemainingTime, 0.0f))));
	}
}

