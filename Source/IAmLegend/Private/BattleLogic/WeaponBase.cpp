// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleLogic/WeaponBase.h"
#include "WeaponDataAsset.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false; // 시작하자마자 틱이 도는 것을 방지

	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 평소엔 충돌 없음
	Mesh->SetCollisionResponseToAllChannels(ECR_Ignore);       // 모든 채널 무시
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	WeaponInitFromData();

	// 추후에 WeaponDataAsset에서 초기화 하는 것으로 변경 예정입니다.
	Range = 200.f; 
}

void AWeaponBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	WeaponInitFromData();
}

void AWeaponBase::WeaponInitFromData()
{
	if (!ItemData) return;

	InitFromData();

	// WeaponDataAsset에서 추가로 초기화할 부분을 여기에 작성하면 됩니다.
	// 현재는 WeaponDataAsset에 Damage만 작성되어 있어 Damage를 초기화하는 부분만 작성했습니다.
	if(UWeaponDataAsset* WeaponData = Cast<UWeaponDataAsset>(ItemData))
	{
		if (WeaponData->Damage > 0.f) 
		{
			Damage = WeaponData->Damage;
		}
		
	}
	
}

void AWeaponBase::WeaponAttack()
{
	Attack();
}

void AWeaponBase::SubAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("Sub Attack!"));
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

