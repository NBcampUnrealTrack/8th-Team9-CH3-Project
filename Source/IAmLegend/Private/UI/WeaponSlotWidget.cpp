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
	auto UpdateSlotIcon = [](UImage* TargetImage, AWeaponBase* WeaponActor)
		{
			if (TargetImage)
			{
				// 무기 액터가 실제로 존재하고(nullptr 방어), 그 무기에 아이콘 이미지가 등록되어 있다면
				if (WeaponActor && WeaponActor->WeaponIcon)
				{
					TargetImage->SetBrushFromTexture(WeaponActor->WeaponIcon);
					TargetImage->SetVisibility(ESlateVisibility::HitTestInvisible);
				}
				else
				{
					// 해당 슬롯에 무기가 없으면 슬롯 이미지를 숨김 처리
					TargetImage->SetVisibility(ESlateVisibility::Hidden);
				}
			}
		};

	// 캐릭터의 기본 무기 세팅 목록 가져와서 등록
	const TMap<EWeaponSlot, AWeaponBase*>& ActiveWeaponSlots = PlayerCharacter->GetWeaponSlots();

	// 1번 근접 무기 슬롯 실시간 체크 및 UI 업데이트
	AWeaponBase* MeleeWeapon = ActiveWeaponSlots.Contains(EWeaponSlot::Melee) ? ActiveWeaponSlots[EWeaponSlot::Melee] : nullptr;
	UpdateSlotIcon(Slot1_Image, MeleeWeapon);

	// 2번 원거리 무기 슬롯 실시간 체크 및 UI 업데이트
	AWeaponBase* RangedWeapon = ActiveWeaponSlots.Contains(EWeaponSlot::Ranged) ? ActiveWeaponSlots[EWeaponSlot::Ranged] : nullptr;
	UpdateSlotIcon(Slot2_Image, RangedWeapon);

	// 3번 단검 무기 슬롯 실시간 체크 및 UI 업데이트
	AWeaponBase* DaggerWeapon = ActiveWeaponSlots.Contains(EWeaponSlot::Dagger) ? ActiveWeaponSlots[EWeaponSlot::Dagger] : nullptr;
	UpdateSlotIcon(Slot3_Image, DaggerWeapon);
}