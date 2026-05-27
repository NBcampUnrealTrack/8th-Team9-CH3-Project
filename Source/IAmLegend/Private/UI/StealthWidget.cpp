// StealthWidget.cpp

#include "UI/StealthWidget.h"
#include "Components/Image.h"
#include "Character/HanPlayerCharacter.h"
#include "Materials/MaterialInstanceDynamic.h"

void UStealthWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 동적 머티리얼 인스턴스를 가져오기
	if (QKeyIcon)
	{
		QKeyMaterial = QKeyIcon->GetDynamicMaterial();
	}
	if (StealthIcon)
	{
		StealthMaterial = StealthIcon->GetDynamicMaterial();
	}
}

void UStealthWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	AHanPlayerCharacter* PlayerCharacter = Cast<AHanPlayerCharacter>(GetOwningPlayerPawn());
	if (!PlayerCharacter || !QKeyMaterial || !StealthMaterial) return;

	bool bIsStealth = PlayerCharacter->bIsStealth;
	float CooldownRatio = PlayerCharacter->GetStealthCooldownRatio();

	float CurrentSaturation = 1.0f;
	float CurrentPercent = 1.0f;

	// 상태에 따라 수치 결정
	if (bIsStealth)
	{
		CurrentSaturation = 1.0f;
		CurrentPercent = 1.0f;
	}
	else if (CooldownRatio < 1.0f)
	{
		CurrentSaturation = 0.2f; // 어둡게
		CurrentPercent = CooldownRatio; // 차오르는 비율
	}

	// 결정된 수치를 두 머티리얼에 동시에 똑같이 적용
	QKeyMaterial->SetScalarParameterValue(FName("Saturation"), CurrentSaturation);
	QKeyMaterial->SetScalarParameterValue(FName("Percent"), CurrentPercent);

	StealthMaterial->SetScalarParameterValue(FName("Saturation"), CurrentSaturation);
	StealthMaterial->SetScalarParameterValue(FName("Percent"), CurrentPercent);
}
