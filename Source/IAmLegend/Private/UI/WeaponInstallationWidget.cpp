// WeaponInstallationWidget.cpp


#include "UI/WeaponInstallationWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Character/HanPlayerCharacter.h"
#include "BattleLogic/Weapon/RangedWeaponBase.h"
#include "WeaponDataAsset.h"
// #include "InventoryComponent.h" // 인벤토리 컴포넌트 헤더

void UWeaponInstallationWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 플레이어 캐릭터 가져오기
	AHanPlayerCharacter* PlayerCharacter = Cast<AHanPlayerCharacter>(GetOwningPlayerPawn());
	if (PlayerCharacter)
	{
		AWeaponBase* EquippedWeapon = PlayerCharacter->GetEquippedWeapon();

		if (EquippedWeapon)
		{
			// 무기 이미지 업데이트 (WeaponDataAsset에 아이콘 정보가 있다면 연결)
			/* UWeaponDataAsset* WeaponData = Cast<UWeaponDataAsset>(EquippedWeapon->ItemData);
			if (WeaponData && WeaponData->ItemIcon)
			{
				WeaponImage->SetBrushFromTexture(WeaponData->ItemIcon);
			}
			*/

			// 2. 사격 무기일 경우 총알 수치 업데이트
			if (ARangedWeaponBase* RangedWeapon = Cast<ARangedWeaponBase>(EquippedWeapon))
			{
				CurrentAmmoText->SetVisibility(ESlateVisibility::Visible);
				TotalAmmoText->SetVisibility(ESlateVisibility::Visible);

				// 현재 장전된 총알
				CurrentAmmoText->SetText(FText::AsNumber(RangedWeapon->GetCurrentAmmo()));

				// 전체 남은 총알 (인벤토리에서 RequiredAmmoItem의 개수를 가져오는 로직 연결 필요)
				// int32 TotalAmmo = PlayerCharacter->GetInventoryComponent()->GetItemCount(WeaponData->RequiredAmmoItem);
				// TotalAmmoText->SetText(FText::AsNumber(TotalAmmo));
			}
			else
			{
				// 근접 무기일 경우 총알 텍스트를 숨김
				CurrentAmmoText->SetVisibility(ESlateVisibility::Hidden);
				TotalAmmoText->SetVisibility(ESlateVisibility::Hidden);
			}
		}

		// 수류탄 개수 업데이트 (인벤토리에서 수류탄 아이템의 개수를 가져오는 로직 연결 필요)
		// int32 GrenadeCount = PlayerCharacter->GetInventoryComponent()->GetItemCount(수류탄데이터에셋);
		// GrenadeCountText->SetText(FText::AsNumber(GrenadeCount));
	}
}