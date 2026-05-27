// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleLogic/WeaponBase.h"
#include "WeaponDataAsset.h"
#include "Character/HanPlayerCharacter.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false; // 시작하자마자 틱이 도는 것을 방지
	WeaponSlot = EWeaponSlot::None; // 기본 무기 슬롯 설정

	// 무기는 메시를 사용하지 않습니다.
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 평소엔 충돌 없음
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);       // 모든 채널 무시
	Mesh->SetHiddenInGame(true); // 게임에서는 메시 숨김
	
	// 무기는 SkeletalMeshComponent를 사용합니다
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(RootComponent);
	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 평소엔 충돌 없음
	SkeletalMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore); // 모든 채널 무시

	OwnerCharacter = nullptr;
	WeaponIcon = nullptr;

	// 기본값 설정
	Damage = 10.0f; 
	Range = 1000.0f;
	WeaponType = EWeaponType::None;
	WeaponSlot = EWeaponSlot::None;

}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

}

void AWeaponBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

}

void AWeaponBase::WeaponInitFromData()
{
	InitFromData();

	// 오너 캐릭터는 이제 WeaponInitFromData에서 설정합니다.
	OwnerCharacter = Cast<AHanPlayerCharacter>(GetOwner());

	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon equipped by a non-player character!"));
	}

	if (!ItemData) return;

	if(UWeaponDataAsset* WeaponData = Cast<UWeaponDataAsset>(ItemData))
	{
		Damage = WeaponData->Damage;
		Range = WeaponData->Range;
		WeaponType = WeaponData->WeaponType;
		WeaponSlot = WeaponData->WeaponSlot;
		
		if (!WeaponData->WeaponSkeletalMesh.IsNull())
		{
			USkeletalMesh* LoadedMesh = WeaponData->WeaponSkeletalMesh.LoadSynchronous();
			if (LoadedMesh)
			{
				SkeletalMesh->SetSkeletalMesh(LoadedMesh);
			}

		}
		if (WeaponData->Attack_1_Montage)
		{
			Attack_1_Montage = WeaponData->Attack_1_Montage;
		}
		
		if(WeaponData->Attack_2_Montage)
		{
			Attack_2_Montage = WeaponData->Attack_2_Montage;
		}

		if(WeaponData->Reload_Montage)
		{
			Reload_Montage = WeaponData->Reload_Montage;
		}

		if(WeaponData->WeaponIcon)
		{
			WeaponIcon = WeaponData->WeaponIcon;
		}
	}
}

void AWeaponBase::StartWeaponAttack()
{
	Attack();
}

void AWeaponBase::StopWeaponAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("Weapon Attack Stopped!"));
}

void AWeaponBase::DestroyWeapon()
{
	// 무기를 소유한 액터에서 분리
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetOwner(nullptr);

	// 충돌과 틱을 비활성화하여 더 이상 상호작용하지 않도록 설정
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
	
	// 무기 액터를 파괴
	Destroy();
}

EWeaponType AWeaponBase::GetWeaponType() const
{
	return WeaponType;
}

EWeaponSlot AWeaponBase::GetWeaponSlot() const
{
	return WeaponSlot;
}

// 애니메이션 노티파이 함수들은 자식에서 오버라이드하여 자신의 공격을 종료합니다.
void AWeaponBase::AnimNotify_EndAttack_1()
{
	UE_LOG(LogTemp, Warning, TEXT("Attack 1 Animation Ended!"));
}

void AWeaponBase::AnimNotify_EndAttack_2()
{
	UE_LOG(LogTemp, Warning, TEXT("Attack 2 Animation Ended!"));
}

void AWeaponBase::AnimNotify_EndReload()
{
	UE_LOG(LogTemp, Warning, TEXT("Reload Animation Ended!"));
}