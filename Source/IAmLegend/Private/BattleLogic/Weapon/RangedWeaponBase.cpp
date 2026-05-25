// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleLogic/Weapon/RangedWeaponBase.h"
#include "WeaponDataAsset.h"
#include "Character/HanPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Item/InventoryComponent.h"
#include "Item/ItemDataAsset.h"
#include "BattleLogic/Weapon/DataAssets/RangedWeaponDataAsset.h"
#include "BattleLogic/Attachment/RangedAttachmentComponent.h"

#define ATTACK_TRACE_CHANNEL ECC_GameTraceChannel1

ARangedWeaponBase::ARangedWeaponBase()
{
	AttachmentComponent = CreateDefaultSubobject<URangedAttachmentComponent>(TEXT("AttachmentComponent"));

	// 기본 값 설정
	WeaponType = EWeaponType::TwoHandedRanged; // 무기 타입 설정
	WeaponSlot = EWeaponSlot::Ranged; // 무기 슬롯 설정
	Range = 10000.f;	// 사거리 10000
	FireRate = 600.f;	// 분당 600발
	MaxAmmo = 30;
	RecoilAmount = 0.05f;
	BaseSpreadAngle = 0.f;
	MaxSpreadAngle = 3.f;
	SpreadPerShot = 0.4f;		// 탄 퍼짐과 회복 속도는 두 배 정도가 적당한 것 같습니다. 추후 UI에서 확인 하면서 조정하면 좋을 것 같습니다.
	RecoverySpreadSpeed = 0.8f;	// 무기 별로 다르게 설정가능 합니다
	MuzzleSocketName = "MuzzleSocket";
	MeleeAttackCooldown = 1.0f;
	MeleeAttackBoxExtent = FVector(1.f, 40.f, 90.f);
	MeleeAttackRange = 100.f;

	bIsReloading = false;
	bIsCoolDown = false;
	bIsPressingAttack = false;
	bIsMeleeAttacking = false;

	// WeaponInitFromData()에서 다시 계산
	FireInterval = 60.f / FireRate;
	CoolDownTime = FireInterval - 0.015f; 
	CurrentAmmo = MaxAmmo;
	CurrentSpreadAngle = BaseSpreadAngle;
	
}

void ARangedWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARangedWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!SkeletalMesh || !OwnerCharacter) return;

	if (bIsMeleeAttacking)
	{
		MeleeAttackTrace(); // 근접 공격 판정 수행
	}
}

void ARangedWeaponBase::StartWeaponAttack()
{
	if (bIsPressingAttack) return; // 이미 공격 중이면 무시

	Super::StartWeaponAttack();

	bIsPressingAttack = true;

	// 캐릭터가 조준한 상태면 발사 로직
	if(OwnerCharacter->IsAiming())
	{
		if (CurrentAmmo <= 0)
		{
			Reload();
			return;
		}

		if (!CanFire())
		{
			return;
		}

		Fire();

		GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ARangedWeaponBase::HandleFire, FireInterval, true);
	}

	// 조준하지 않은 상태에서는 무기별 근접 공격 로직
	else if (CanMeleeAttack())
	{
		ExecuteMeleeAttack();
	}
}

void ARangedWeaponBase::StopWeaponAttack()
{
	Super::StopWeaponAttack();
	bIsPressingAttack = false;
	GetWorldTimerManager().ClearTimer(FireRateTimerHandle);
}

void ARangedWeaponBase::Reload()
{
	GetWorldTimerManager().ClearTimer(FireRateTimerHandle);

	if (!OwnerCharacter || bIsReloading || CurrentAmmo >= MaxAmmo) return; // 이미 재장전 중이거나 탄약이 가득 차 있으면 무시

	// 인벤토리에서 총알 가져와서 사용하기 - 김민성
	int32 TotalAmmo = 0;
	TArray<FItemSlot>& Inventory = OwnerCharacter->GetInventoryComponent()->GetActualInventory();
	for (const FItemSlot& Slot : Inventory)
	{
		if (Slot.ItemData && Slot.ItemData->Category == EItemCategory::Ammo)
		{
			TotalAmmo += Slot.Quantity;
		}
	}

	// 총알 = 0일 시 장전 취소 - 김민성
	if (TotalAmmo <= 0) return;

	// 재장전 애니메이션 재생
	OwnerCharacter->PlayReloadMontage();
	bIsReloading = true;

	UE_LOG(LogTemp, Log, TEXT("Started reloading weapon: %s"), *GetName());
}

void ARangedWeaponBase::FinishReload()
{
	if (!OwnerCharacter) return;
	// ------코드 추가 [김민성]-----------
	// 채워야 할 총알 개수 계산 및 인벤토리에서 실제 차감
	int32 AmmoNeeded = MaxAmmo - CurrentAmmo;
	int32 AmmoToReload = 0;

	TArray<FItemSlot>& Inventory = OwnerCharacter->GetInventoryComponent()->GetActualInventory();

	// 인벤토리 뒤에서부터 검색하여 총알 차감
	for (int32 i = Inventory.Num() - 1; i >= 0; i--)
	{
		if (Inventory[i].ItemData && Inventory[i].ItemData->Category == EItemCategory::Ammo)
		{
			// 슬롯에 있는 총알이 필요량보다 많을 때
			if (Inventory[i].Quantity > AmmoNeeded)
			{
				Inventory[i].Quantity -= AmmoNeeded;
				AmmoToReload += AmmoNeeded;
				AmmoNeeded = 0;
				break;
			}
			// 슬롯에 있는 총알이 부족하거나 딱 맞을 때 (남은거 다 긁어모으고 슬롯 삭제)
			else
			{
				AmmoToReload += Inventory[i].Quantity;
				AmmoNeeded -= Inventory[i].Quantity;
				Inventory.RemoveAt(i);
			}
		}
	}

	// 가방에서 꺼내온 총알 수만큼만 탄창에 추가
	CurrentAmmo += AmmoToReload;
	bIsReloading = false;
	// -----------------

	// CurrentAmmo = MaxAmmo; // 탄약 수 초기화
	// bIsReloading = false;

	UE_LOG(LogTemp, Log, TEXT("Finished reloading weapon: %s, Current Ammo: %d"), *GetName(), CurrentAmmo);
}

void ARangedWeaponBase::HandleFire()
{
	UE_LOG(LogTemp, Log, TEXT("Handling fire for weapon: %s"), *GetName());
	if(CurrentAmmo <= 0)
	{
		Reload();
		return;
	}

	if(!CanFire())
	{
		GetWorldTimerManager().ClearTimer(FireRateTimerHandle);
		return;
	}

	if(bIsPressingAttack)
	{
		Fire();
	}
	else
	{
		GetWorldTimerManager().ClearTimer(FireRateTimerHandle);
	}
}

void ARangedWeaponBase::Fire()
{
	if (!OwnerCharacter || !SkeletalMesh) return;

	// 무기에서 발사 시 애니메이션 몽타주 재생
	OwnerCharacter->PlayAttackMontage_2();
	if(FireAnimSequence && SkeletalMesh)
	{
		SkeletalMesh->PlayAnimation(FireAnimSequence, false);
	}

	CurrentAmmo--;
	bIsCoolDown = true;
	GetWorldTimerManager().SetTimer(CoolDownTimerHandle, this, &ARangedWeaponBase::FinishCooldown, CoolDownTime, false);

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

	// 발사 후 반동과 탄 퍼짐 증가 적용
	ApplyRecoil();
	ApplySpread();

	// 탄 퍼짐 회복 타이머 시작
	GetWorldTimerManager().SetTimer(SpreadRecoveryTimerHandle, this, &ARangedWeaponBase::RecoverSpread, 0.01f, true);
}

void ARangedWeaponBase::ApplyRecoil()
{
	// 반동 적용
	if (!OwnerCharacter) return;

	FRotator CurrentControlRotation = OwnerCharacter->GetControlRotation();
	float RecoilPitch = FMath::RandRange(RecoilAmount / 2, RecoilAmount); // 수직 반동량
	
	OwnerCharacter->AddControllerPitchInput(-RecoilPitch);

	// 수평 반동은 추후 추가
	// float RecoilYaw = FMath::RandRange(-RecoilAmount / 2, RecoilAmount / 2); // 수평 반동량
	//OwnerCharacter->AddControllerYawInput(RecoilYaw);

}

void ARangedWeaponBase::FinishCooldown()
{
	bIsCoolDown = false;

	UE_LOG(LogTemp, Log, TEXT("Weapon cooldown finished: %s"), *GetName());
}

bool ARangedWeaponBase::CanFire() const
{
	// 재장전 중이 아니고, 쿨다운 중이 아니며, 탄약이 남아있고, 조준 중일 때만 발사 가능
	return !bIsReloading && !bIsCoolDown && CurrentAmmo > 0 && OwnerCharacter->IsAiming(); 
}

void ARangedWeaponBase::ApplySpread()
{
	CurrentSpreadAngle = FMath::Min(CurrentSpreadAngle + SpreadPerShot, MaxSpreadAngle);
}

void ARangedWeaponBase::RecoverSpread()
{
	CurrentSpreadAngle = FMath::FInterpTo(CurrentSpreadAngle, BaseSpreadAngle, 0.01f, RecoverySpreadSpeed);

	if (FMath::IsNearlyEqual(CurrentSpreadAngle, BaseSpreadAngle, 0.01f))
	{
		CurrentSpreadAngle = BaseSpreadAngle;
		GetWorldTimerManager().ClearTimer(SpreadRecoveryTimerHandle);
	}
}

void ARangedWeaponBase::CalculateTrace(FVector& Start, FVector& End)
{
	if (!OwnerCharacter || !OwnerCharacter->GetController()) return;
	APlayerController* PlayerController = Cast<APlayerController>(OwnerCharacter->GetController());

	if (!PlayerController) return;

	FVector CameraLocation;
	FRotator CameraRotation;

	PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
	FVector CameraEnd = CameraLocation + (CameraRotation.Vector() * Range);

	FHitResult ScreenTraceHit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter); 
	Params.AddIgnoredActor(this);           

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		ScreenTraceHit,
		CameraLocation,
		CameraEnd,
		ECC_Visibility,
		Params
	);
	FVector TargetPoint = bHit ? ScreenTraceHit.ImpactPoint : CameraEnd;

	FRotator Rotation;
	OwnerCharacter->GetActorEyesViewPoint(Start, Rotation);
	
	// 목적지 계산
	FVector DirectionToTarget = (TargetPoint - Start).GetSafeNormal();

	// 탄 퍼짐 계산
	FVector Spread = FMath::VRandCone(DirectionToTarget, FMath::DegreesToRadians(CurrentSpreadAngle));

	// Range는 AWeaponBase에서 상속받은 사거리입니다. 현재는 블루프린트 내에서 값을 수정해서 사용하고 있습니다.
	End = Start + Spread * Range;
}

bool ARangedWeaponBase::AttackTrace(FVector& StartLocation, FVector& EndLocation, FHitResult& HitResult)
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(OwnerCharacter);

	return GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ATTACK_TRACE_CHANNEL, QueryParams);
}

void ARangedWeaponBase::ProcessHit(const FHitResult& HitResult)
{
	AActor* HitActor = HitResult.GetActor();
	if (HitActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitActor->GetName());
		UGameplayStatics::ApplyDamage(HitActor, Damage, OwnerCharacter->GetInstigatorController(), this, nullptr);
	}
}

//--------------------------------------------------------
// 근접 공격 로직

void ARangedWeaponBase::ExecuteMeleeAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("Executing melee attack for weapon: %s"), *GetName());

	// 근접 공격 애니메이션 재생
	OwnerCharacter->PlayAttackMontage_1();

	// 근접 공격을 시작하면 발사와 재장전 타이머를 모두 무효화
	FireRateTimerHandle.Invalidate(); // 발사 타이머 무효화
	bIsReloading = false;			  // 재장전 상태 해제

	bIsMeleeAttacking = true;
	SetActorTickEnabled(true);	// 틱을 활성화해 타격 판정 시작
}

void ARangedWeaponBase::StopMeleeAttack()
{
	bIsMeleeAttacking = false;	// 근접 공격 종료
	bIsCoolDown = true;			// 근접 공격 후에도 쿨다운 적용
	SetActorTickEnabled(false); // 틱 비활성화해 타격 판정 종료
	HitActors.Empty();			// 타격한 액터 목록 초기화

	GetWorldTimerManager().SetTimer(
		CoolDownTimerHandle,
		this,
		&ARangedWeaponBase::FinishCooldown,
		MeleeAttackCooldown,
		false
	);
	UE_LOG(LogTemp, Warning, TEXT("Finished melee attack for weapon: %s"), *GetName());
}

void ARangedWeaponBase::MeleeAttackTrace()
{
	// 근접 공격 로직은 자식에서 구현
}

void ARangedWeaponBase::ProcessMeleeHits(const TArray<FHitResult>& HitResults)
{
	// 근접 공격 타격 판정 처리 로직은 자식에서 구현
}

bool ARangedWeaponBase::CanMeleeAttack() const
{
	return !bIsCoolDown && !bIsMeleeAttacking && !OwnerCharacter->IsAiming(); 
}

// --------------------------------------------------------
// 애니메이션 노티파이 함수들 (애니메이션 몽타주에서 호출)

void ARangedWeaponBase::AnimNotify_EndAttack_1()
{
	Super::AnimNotify_EndAttack_1();
	StopMeleeAttack();
}

void ARangedWeaponBase::AnimNotify_EndReload()
{
	Super::AnimNotify_EndReload();
	FinishReload();
}

// --------------------------------------------------------
// 데이터 에셋에서 초기화
void ARangedWeaponBase::WeaponInitFromData()
{
	Super::WeaponInitFromData();

	if (!ItemData) return;

	if (URangedWeaponDataAsset* RangedWeaponData = Cast<URangedWeaponDataAsset>(ItemData))
	{
		FireRate = RangedWeaponData->FireRate;
		MaxAmmo = RangedWeaponData->MaxAmmo;
		RecoilAmount = RangedWeaponData->RecoilAmount;
		SpreadPerShot = RangedWeaponData->SpreadPerShot;
		BaseSpreadAngle = RangedWeaponData->BaseSpreadAngle;
		MaxSpreadAngle = RangedWeaponData->MaxSpreadAngle;
		RecoverySpreadSpeed = RangedWeaponData->RecoverySpreadSpeed;
		MeleeAttackCooldown = RangedWeaponData->MeleeAttackCooldown;
		MeleeAttackBoxExtent = RangedWeaponData->MeleeAttackBoxExtent;
		MeleeAttackRange = RangedWeaponData->MeleeAttackRange;
		MuzzleSocketName = RangedWeaponData->MuzzleSocketName;
		FireAnimSequence = RangedWeaponData->FireAnimSequence;
		AttachmentSlots = RangedWeaponData->AttachmentSlots;

		// 데이터 에셋에서 불러온 값으로 발사 간격과 쿨다운 시간 계산
		FireInterval = 60.f / FireRate;
		CoolDownTime = FireInterval - 0.015f;
		CurrentAmmo = MaxAmmo;
		CurrentSpreadAngle = BaseSpreadAngle;
		
	}
}

URangedAttachmentComponent* ARangedWeaponBase::GetRangedAttachmentComponent() const
{
	return AttachmentComponent;
}