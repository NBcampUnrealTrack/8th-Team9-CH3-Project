// WeaponSlotWidget.cpp

#include "UI/WeaponSlotWidget.h"

#include "Components/Image.h"
#include "Character/HanPlayerCharacter.h"
#include "BattleLogic/WeaponBase.h"

void UWeaponSlotWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	AHanPlayerCharacter* PlayerCharacter = Cast<AHanPlayerCharacter>(GetOwningPlayerPawn());
	if (!PlayerCharacter) return;

	// 내부 람다 함수 (CDO에서 아이콘 추출)
	auto UpdateSlotIcon = [](UImage* TargetImage, TSubclassOf<AWeaponBase> WeaponClass)
		{
			if (TargetImage)
			{
				if (WeaponClass)
				{
					// 스폰되지 않은 클래스 원본(CDO)에서 직접 아이콘 변수에 접근
					AWeaponBase* CDO = WeaponClass->GetDefaultObject<AWeaponBase>();
					if (CDO && CDO->WeaponIcon)
					{
						TargetImage->SetBrushFromTexture(CDO->WeaponIcon);
						TargetImage->SetVisibility(ESlateVisibility::Visible);
						return;
					}
				}
				// 무기가 비어있으면 투명하게 처리
				TargetImage->SetVisibility(ESlateVisibility::Hidden);
			}
		};

	// 캐릭터의 기본 무기 세팅 목록 가져와서 등록
	TMap<EWeaponSlot, TSubclassOf<AWeaponBase>> WeaponClasses = PlayerCharacter->GetDefaultWeaponClasses();

	if (WeaponClasses.Contains(EWeaponSlot::Melee))
		UpdateSlotIcon(Slot1_Image, WeaponClasses[EWeaponSlot::Melee]);

	if (WeaponClasses.Contains(EWeaponSlot::Ranged))
		UpdateSlotIcon(Slot2_Image, WeaponClasses[EWeaponSlot::Ranged]);

	if (WeaponClasses.Contains(EWeaponSlot::Dagger))
		UpdateSlotIcon(Slot3_Image, WeaponClasses[EWeaponSlot::Dagger]);
}