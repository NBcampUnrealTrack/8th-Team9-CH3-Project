// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleLogic/Weapon/RangedWeaponBase.h"
#include "WeaponDataAsset.h"
#include "Character/HanPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

#define ATTACK_TRACE_CHANNEL ECC_GameTraceChannel1

ARangedWeaponBase::ARangedWeaponBase()
{
	// 초기값 설정 (추후에 WeaponDataAsset에서 초기화 하는 것으로 변경 예정입니다.)
	Range = 10000.f;	// 사거리 10000
	FireRate = 600.f;	// 분당 600발
	MaxAmmo = 30;
	ReloadTime = 2.0f;
	RecoilAmount = 0.05f;
	BaseSpreadAngle = 0.f;
	MaxSpreadAngle = 3.f;
	
	AimingBonus = 0.5f;			// 조준 시 퍼짐이 절반으로 감소
	SpreadPerShot = 0.4f;		// 탄 퍼짐과 회복 속도는 두 배 정도가 적당한 것 같습니다. 추후 UI에서 확인 하면서 조정하면 좋을 것 같습니다.
	RecoverySpreadSpeed = 0.8f;	// 무기 별로 다르게 설정가능 합니다

	MuzzleSocketName = "MuzzleSocket";
	bIsReloading = false;
	bIsCoolDown = false;
	bIsPressingAttack = false;

	// BeginPlay에서 초기화하는 값들
	FireInterval = 60.f / FireRate;
	CoolDownTime = FireInterval - 0.01f; 
	CurrentAmmo = MaxAmmo;
	CurrentSpreadAngle = BaseSpreadAngle;
	
}

void ARangedWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	WeaponInitFromData();

	FireInterval = 60.f / FireRate;			// 발사 간격 계산
	CoolDownTime = FireInterval - 0.01f;	// 발사 후 쿨다운 시간 계산 (오토와 시간이 완전히 동일하면 타이머가 제대로 작동하지 않을 수 있으므로 약간의 여유를 둡니다.)
	CurrentAmmo = MaxAmmo;					// 초기 탄약 수 설정
	CurrentSpreadAngle = BaseSpreadAngle;	// 초기 퍼짐 각도 설정
	
}

void ARangedWeaponBase::WeaponInitFromData()
{
	Super::WeaponInitFromData();

	// WeaponDataAsset에서 추가로 초기화할 부분을 여기에 작성하면 됩니다.
	if(UWeaponDataAsset* WeaponData = Cast<UWeaponDataAsset>(ItemData))
	{
		
	}
}


void ARangedWeaponBase::StartWeaponAttack()
{
	if (bIsPressingAttack) return; // 이미 공격 중이면 무시

	Super::StartWeaponAttack();

	bIsPressingAttack = true;

	if (CurrentAmmo <= 0)
	{
		Reload();
		return;
	}

	if(!CanFire())
	{
		return;
	}

	Fire();

	GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ARangedWeaponBase::HandleFire, FireInterval, true);
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

	if (bIsReloading) return; // 이미 재장전 중이면 무시

	bIsReloading = true;
	GetWorldTimerManager().SetTimer(ReloadTimerHandle, this, &ARangedWeaponBase::FinishReload, ReloadTime, false);

	UE_LOG(LogTemp, Log, TEXT("Started reloading weapon: %s"), *GetName());
}

void ARangedWeaponBase::FinishReload()
{
	GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
	CurrentAmmo = MaxAmmo; // 탄약 수 초기화
	bIsReloading = false;

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
	CurrentAmmo--;
	bIsCoolDown = true;
	GetWorldTimerManager().SetTimer(CoolDownTimerHandle, this, &ARangedWeaponBase::FinishCooldown, CoolDownTime, false);

	if (!OwnerCharacter) return;
	
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
	ApplySpread(OwnerCharacter->IsAiming());

	// 탄 퍼짐 회복 타이머 시작
	GetWorldTimerManager().SetTimer(SpreadRecoveryTimerHandle, this, &ARangedWeaponBase::RecoverSpread, 0.01f, true);

	// 디버그 라인
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.0f, 0, 1.0f);
	UE_LOG(LogTemp, Log, TEXT("Fired weapon: %s, Current Ammo: %d, Current Spread: %f"), *GetName(), CurrentAmmo, CurrentSpreadAngle);
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
	// 재장전 중이 아니고, 쿨다운 중이 아니며, 탄약이 남아있는 경우에만 발사 가능
	return !bIsReloading && !bIsCoolDown && CurrentAmmo > 0; 
}

void ARangedWeaponBase::ApplySpread(bool bIsAiming)
{
	float ActualMaxSpread = bIsAiming ? (MaxSpreadAngle * AimingBonus) : MaxSpreadAngle;
	float ActualSpreadPerShot = bIsAiming ? (SpreadPerShot * AimingBonus) : SpreadPerShot;

	CurrentSpreadAngle = FMath::Min(CurrentSpreadAngle + ActualSpreadPerShot, ActualMaxSpread);
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
	if (!OwnerCharacter) return;
	FRotator Rotation;
	OwnerCharacter->GetActorEyesViewPoint(Start, Rotation);

	// 탄 퍼짐 계산
	FVector Spread = FMath::VRandCone(Rotation.Vector(), FMath::DegreesToRadians(CurrentSpreadAngle));

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
