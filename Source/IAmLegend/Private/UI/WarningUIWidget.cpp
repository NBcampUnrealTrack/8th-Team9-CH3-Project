#include "UI/WarningUIWidget.h"

void UWarningUIWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UE_LOG(LogTemp, Warning, TEXT("백신을 보유하고 있지 않습니다."))
	UE_LOG(LogTemp, Warning, TEXT("현재 Opacity: %f"), GetRenderOpacity());
	// 위젯이 뷰포트에 추가되는 순간 자동 실행
	SetRenderOpacity(1.0f);
	FadeElapsed = 0.0f;
	FadeDuration = 3.0f;

	GetWorld()->GetTimerManager().SetTimer(
		FadeTimerHandle,
		this,
		&UWarningUIWidget::UpdateFade,
		0.016f,
		true
	);
}

void UWarningUIWidget::UpdateFade()
{
	FadeElapsed += 0.016f;

	float Alpha = FMath::Clamp(1.0f - (FadeElapsed / FadeDuration), 0.0f, 1.0f);
	SetRenderOpacity(Alpha);

	if (FadeElapsed < 0.1f)
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateFade 실행중 - Alpha: %f"), Alpha);
	}
	if (FadeElapsed >= FadeDuration)
	{
		GetWorld()->GetTimerManager().ClearTimer(FadeTimerHandle);
		RemoveFromParent();
	}
}