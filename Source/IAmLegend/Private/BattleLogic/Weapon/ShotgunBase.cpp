// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleLogic/Weapon/ShotgunBase.h"
#include "WeaponDataAsset.h"
#include "Character/HanPlayerCharacter.h"
#include "Item/InventoryComponent.h"
#include "Item/ItemDataAsset.h"
#include "Kismet/GameplayStatics.h"
#include "BattleLogic/Weapon/DataAssets/ShotgunDataAsset.h"

AShotgunBase::AShotgunBase()
{
	// 초기값 설정 (추후에 WeaponDataAsset에서 초기화 하는 것으로 변경 예정입니다.)
	WeaponType = EWeaponType::Shotgun; // 무기 타입 설정
	FireRate = 75.f;	// 분당 75발, 0.8초마다 발사
	MaxAmmo = 5;
	RecoilAmount = 0.1f;	// 현재 반동은 자동으로 회복되지 않으므로 고민을 좀 해봐야 할 것 같습니다.
	BaseSpreadAngle = 5.5f;	// 샷건은 기본적으로 탄퍼짐을 크게 둡니다.
	MaxSpreadAngle = 5.5f;	
	SpreadPerShot = 0.f;		// 샷건은 발당 탄 퍼짐이 증가하지 않도록 설정합니다. 대신 BaseSpreadAngle이 이미 넓게 설정되어 있습니다.
	RecoverySpreadSpeed = 0.0f;	// 무기 별로 다르게 설정가능 합니다

	PelletsPerShot = 8; // 샷건 발당 발사되는 펠릿 수 설정
	AmmoPerReload = 1;	// 재장전 시 장전되는 탄약 수 설정 (탄창을 사용하는 샷건을 고려해 추가했습니다.)
}

void AShotgunBase::BeginPlay()
{
	Super::BeginPlay();
}

void AShotgunBase::StartWeaponAttack()
{
	// 샷건은 재장전 중 공격 시 취소하고 발사
	if (bIsReloading && CurrentAmmo > 0)
	{
		bIsReloading = false;
	}

	Super::StartWeaponAttack();
}

void AShotgunBase::FinishReload()
{
	// ------코드 추가 [김민성]-----------
	if (!OwnerCharacter) return;

	// 인벤토리에서 1발(AmmoPerReload)만 빼오기 
	int32 AmmoToReload = 0;
	TArray<FItemSlot>& Inventory = OwnerCharacter->GetInventoryComponent()->GetActualInventory();

	for (int32 i = Inventory.Num() - 1; i >= 0; i--)
	{
		if (Inventory[i].ItemData && Inventory[i].ItemData->Category == EItemCategory::Ammo)
		{
			if (Inventory[i].Quantity >= AmmoPerReload)
			{
				Inventory[i].Quantity -= AmmoPerReload;
				AmmoToReload = AmmoPerReload;
				break;
			}
			else
			{
				AmmoToReload = Inventory[i].Quantity;
				Inventory.RemoveAt(i);
				break;
			}
		}
	}

	CurrentAmmo += AmmoToReload; // 탄약 추가
	UE_LOG(LogTemp, Warning, TEXT("Shotgun Reloading... Current Ammo: %d / %d"), CurrentAmmo, MaxAmmo);

	UAnimInstance* OwnerAnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (!OwnerAnimInstance) return;

	UAnimMontage* CurrentMontage = OwnerAnimInstance->GetCurrentActiveMontage();
	if (!CurrentMontage) return;

	if (CurrentAmmo < MaxAmmo && AmmoToReload > 0)
	{
		OwnerAnimInstance->Montage_SetNextSection(FName("Loop"), FName("Loop"), CurrentMontage);
		UE_LOG(LogTemp, Log, TEXT("Keep Reloading... Next: Loop"));
	}
	else // 탄창이 꽉 찼거나 가방에 더 이상 여분 총알이 없으면 장전 강제 종료
	{
		OwnerAnimInstance->Montage_SetNextSection(FName("Loop"), FName("End"), CurrentMontage);
		bIsReloading = false;
		UE_LOG(LogTemp, Warning, TEXT("Shotgun Reload complete. Next: End"));
	}
}

void AShotgunBase::Fire()
{
	if (!OwnerCharacter || !SkeletalMesh) return;

	OwnerCharacter->PlayAnimMontage(Attack_2_Montage);
	if (FireAnimSequence && SkeletalMesh)
	{
		SkeletalMesh->PlayAnimation(FireAnimSequence, false);
	}

	CurrentAmmo--;
	bIsCoolDown = true;
	GetWorldTimerManager().SetTimer(CoolDownTimerHandle, this, &ARangedWeaponBase::FinishCooldown, CoolDownTime, false);

	// 샷건은 여러 개의 펠릿을 동시에 발사
	for (int32 i = 0; i < PelletsPerShot; i++)
	{
		// 탄 발사 시 트레이스 계산
		FVector Start, End;
		CalculateTrace(Start, End);

		// 트레이스 결과 처리
		FHitResult HitResult;
		bool bHit = AttackTrace(Start, End, HitResult);

		// 트레이스에 맞은 액터가 있으면 처리
		if (bHit)
		{
			ProcessHit(HitResult);
		}
	}

	// 발사 후 반동과 탄 퍼짐 증가 적용
	ApplyRecoil();
	ApplySpread();

	// 탄 퍼짐 회복 타이머 시작
	GetWorldTimerManager().SetTimer(SpreadRecoveryTimerHandle, this, &ARangedWeaponBase::RecoverSpread, 0.01f, true);
}

// --------------------------------------------------------
// 데이터 에셋에서 초기화
void AShotgunBase::WeaponInitFromData()
{
	Super::WeaponInitFromData();

	if (UShotgunDataAsset* ShotgunData = Cast<UShotgunDataAsset>(ItemData))
	{
		PelletsPerShot = ShotgunData->PelletsPerShot;
		AmmoPerReload = ShotgunData->AmmoPerReload;
	}
}