// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleLogic/Weapon/ThrowableWeaponBase.h"
#include "WeaponDataAsset.h"
#include "BattleLogic/Projectile/WeaponProjectileBase.h"
#include "Character/HanPlayerCharacter.h"
#include "BattleLogic/TrajectoryComponent.h"
#include "BattleLogic/Weapon/DataAssets/ThrowableWeaponDataAsset.h"

AThrowableWeaponBase::AThrowableWeaponBase()
{
	// 초기값 설정 (추후에 WeaponDataAsset에서 초기화 하는 것으로 변경 예정입니다.)
	WeaponType = EWeaponType::Granade; // 무기 타입 설정
	TrajectoryComp = CreateDefaultSubobject<UTrajectoryComponent>(TEXT("TrajectoryComponent"));
}

void AThrowableWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
	if(TrajectoryComp)
	{
		TrajectoryComp->InitializeTrajectory(ProjectileClass);
	}
}

void AThrowableWeaponBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AThrowableWeaponBase::StartWeaponAttack()
{
	Super::StartWeaponAttack();

	if (!OwnerCharacter) return;

	if (OwnerCharacter->IsAiming())
	{
		ThrowWeapon(); // 조준 상태에서는 무기 투척
	}
}

// 투척 공격
void AThrowableWeaponBase::ThrowWeapon()
{
	if (!ProjectileClass || !OwnerCharacter || !SkeletalMesh) return;

	// 칼날의 끝 위치에서 투사체를 생성
	FVector SpawnLocation = SkeletalMesh->GetSocketLocation(FName("Tip"));

	// 플레이어의 시점에 따라 투사체의 초기 회전을 설정
	FRotator SpawnRotation;							 // 주석이 깨져요 ㅠㅠ
	if (OwnerCharacter)
	{
		SpawnRotation = OwnerCharacter->GetControlRotation();
	}
	else
	{
		SpawnRotation = GetActorRotation();
	}

	// 투사체 생성
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerCharacter;
	SpawnParams.Instigator = OwnerCharacter->GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AWeaponProjectileBase* SpawnedProjectile = GetWorld()->SpawnActor<AWeaponProjectileBase>(
		ProjectileClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);
	OwnerCharacter->UnEquipWeapon(WeaponSlot); // 투척 후 무기 해제
}	

void AThrowableWeaponBase::EnableTrajectory(bool bEnable)
{
	if (TrajectoryComp)
	{
		TrajectoryComp->EnableTrajectory(bEnable);
	}
}

void AThrowableWeaponBase::WeaponInitFromData()
{
	Super::WeaponInitFromData();

	if (!ItemData) return;

	if (UThrowableWeaponDataAsset* ThrowableWeaponData = Cast<UThrowableWeaponDataAsset>(ItemData))
	{
		ProjectileClass = ThrowableWeaponData->ProjectileClass;
	}
}