// PlayerHealthWidget.cpp


#include "PlayerHealthWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UPlayerHealthWidget::UpdateHP(float CurrentHP, float MaxHP)
{
	if (MaxHP <= 0.f) return;

	// 프로그레스 바 퍼센트 채우기
	if (pbHP) {
		float HealthPercent = CurrentHP / MaxHP;
		pbHP->SetPercent(HealthPercent);
	}

	// 체력 text 업데이트
	if (txtHP) {
		FString HPString = FString::Printf(TEXT("%.0f/%.0f"), CurrentHP, MaxHP);
		txtHP->SetText(FText::FromString(HPString));
	}
}