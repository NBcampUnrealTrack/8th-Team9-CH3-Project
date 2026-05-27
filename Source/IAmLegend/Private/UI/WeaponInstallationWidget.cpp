// WeaponInstallationWidget.cpp


#include "UI/WeaponInstallationWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Character/HanPlayerCharacter.h"
#include "BattleLogic/Weapon/RangedWeaponBase.h"
#include "WeaponDataAsset.h"
#include "Item/InventoryComponent.h" // 인벤토리 컴포넌트 헤더
#include "Item/ItemDataAsset.h"
#include "BattleLogic/Weapon/DataAssets/ThrowableWeaponDataAsset.h"

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
			// 무기 이미지 업데이트
			if (EquippedWeapon->WeaponIcon)
			{
				WeaponImage->SetBrushFromTexture(EquippedWeapon->WeaponIcon);
			}
			

			// 2. 사격 무기일 경우 총알 수치 업데이트
			if (ARangedWeaponBase* RangedWeapon = Cast<ARangedWeaponBase>(EquippedWeapon))
			{
				CurrentAmmoText->SetVisibility(ESlateVisibility::Visible);
				TotalAmmoText->SetVisibility(ESlateVisibility::Visible);

				// 현재 장전된 총알
				CurrentAmmoText->SetText(FText::AsNumber(RangedWeapon->GetCurrentAmmo()));

				// 전체 남은 총알 (나중에 총알 종류 늘어나면 해당 함수 사용)
				/*
				if (WeaponData && WeaponData->RequiredAmmoItem)
				{
					int32 TotalAmmo = PlayerCharacter->GetInventoryComponent()->GetItemQuantity(WeaponData->RequiredAmmoItem);
					TotalAmmoText->SetText(FText::AsNumber(TotalAmmo));
				}
				else
				{
					// 데이터가 없으면(총알 = 0) 0으로 표시
					TotalAmmoText->SetText(FText::AsNumber(0));
				}*/
				int32 TotalAmmo = 0;
				TArray<FItemSlot>& Inventory = PlayerCharacter->GetInventoryComponent()->GetActualInventory();

				for (const FItemSlot& InSlot : Inventory)
				{
					if (InSlot.ItemData && InSlot.ItemData->Category == EItemCategory::Ammo)
					{
						TotalAmmo += InSlot.Quantity;
					}
				}

				TotalAmmoText->SetText(FText::AsNumber(TotalAmmo));
			}
			else
			{
				// 근접 무기일 경우 총알 텍스트를 숨김
				CurrentAmmoText->SetVisibility(ESlateVisibility::Hidden);
				TotalAmmoText->SetVisibility(ESlateVisibility::Hidden);
			}
		}

		// 수류탄 개수 업데이트
		int32 GrenadeCount = 0;
		TArray<FItemSlot>& Inventory = PlayerCharacter->GetInventoryComponent()->GetActualInventory();

		for (const FItemSlot& InSlot : Inventory)
		{
			// 인벤토리 아이템 중 수류탄 데이터 에셋(UThrowableWeaponDataAsset)인 것만 찾아 개수 합산
			if (InSlot.ItemData && Cast<UThrowableWeaponDataAsset>(InSlot.ItemData))
			{
				GrenadeCount += InSlot.Quantity;
			}
		}

		if (GrenadeCountText)
		{
			GrenadeCountText->SetText(FText::AsNumber(GrenadeCount));
		}
	}
}