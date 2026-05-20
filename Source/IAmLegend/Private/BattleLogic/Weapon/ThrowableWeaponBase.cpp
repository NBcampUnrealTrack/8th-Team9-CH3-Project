// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleLogic/Weapon/ThrowableWeaponBase.h"
#include "WeaponDataAsset.h"
#include "BattleLogic/Projectile/WeaponProjectileBase.h"
#include "Character/HanPlayerCharacter.h"
#include "BattleLogic/TrajectoryComponent.h"
#include "BattleLogic/Weapon/DataAssets/ThrowableWeaponDataAsset.h"

AThrowableWeaponBase::AThrowableWeaponBase()
{
	// 초기값 설정
	WeaponType = EWeaponType::Granade; // 무기 타입 설정
	TrajectoryComp = CreateDefaultSubobject<UTrajectoryComponent>(TEXT("TrajectoryComponent"));
}

void AThrowableWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
	if(TrajectoryComp)
	{
		TrajectoryComp->InitializeTrajectory(ItemData);
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

	// 캐릭터의 오른손 위치에서 투사체 생성 (무기 소켓 위치 사용)
	FVector SpawnLocation = OwnerCharacter->GetMesh()->GetSocketLocation(FName("WeaponSocket"));

	// 플레이어의 시점에 따라 투사체의 초기 회전을 설정
	FRotator SpawnRotation;							
	if (OwnerCharacter)
	{
		SpawnRotation = OwnerCharacter->GetControlRotation();
	}
	else
	{
		SpawnRotation = GetActorRotation();
	}

	// 투사체 생성
	UThrowableWeaponDataAsset* ThrowableWeaponData = Cast<UThrowableWeaponDataAsset>(ItemData);
	AWeaponProjectileBase* SpawnedProjectile = GetWorld()->SpawnActorDeferred<AWeaponProjectileBase>(
		ThrowableWeaponData->ProjectileClass,                        
		FTransform(SpawnRotation, SpawnLocation),                    
		OwnerCharacter,                                             
		OwnerCharacter->GetInstigator(),                            
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn            
	);

	if (SpawnedProjectile)
	{
		SpawnedProjectile->InitProjectileFromData(ThrowableWeaponData);

		SpawnedProjectile->FinishSpawning(FTransform(SpawnRotation, SpawnLocation));
	}

	
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