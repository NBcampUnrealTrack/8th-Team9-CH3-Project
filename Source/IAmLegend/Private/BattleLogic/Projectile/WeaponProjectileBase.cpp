// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleLogic/Projectile/WeaponProjectileBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BattleLogic/Weapon/DataAssets/ThrowableWeaponDataAsset.h"

// Sets default values
AWeaponProjectileBase::AWeaponProjectileBase()
{
	
	PrimaryActorTick.bCanEverTick = false;

	// 기본값 설정
	CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("SphereComp"));
	CollisionComp->SetCapsuleSize(5.f, 50.f);
	CollisionComp->SetCollisionProfileName(TEXT("Projectile"));
	CollisionComp->SetNotifyRigidBodyCollision(true);
	CollisionComp->OnComponentHit.AddDynamic(this, &AWeaponProjectileBase::OnHit);
	RootComponent = CollisionComp;

	ProjectileMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GravityScale = 1.0f;
	InitialSpeed = 3000.f;
	MaxSpeed = 3000.f;
	bRotationFollowsVelocity = true;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->ProjectileGravityScale = GravityScale;
	ProjectileMovement->InitialSpeed = InitialSpeed;
	ProjectileMovement->MaxSpeed = MaxSpeed;
	ProjectileMovement->bRotationFollowsVelocity = bRotationFollowsVelocity;

	Damage = 20.f;
	LifeSpan = 5.0f;

	bHasHit = false;
}

// Called when the game starts or when spawned
void AWeaponProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeaponProjectileBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AWeaponProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("Projectile hit: %s"), *OtherActor->GetName());
}

void AWeaponProjectileBase::InitProjectileFromData(UThrowableWeaponDataAsset* ThrowableWeaponData)
{
	if (!ThrowableWeaponData) return;

	if (!ThrowableWeaponData->WeaponSkeletalMesh.IsNull())
	{
		if (USkeletalMesh* LoadedMesh = ThrowableWeaponData->WeaponSkeletalMesh.LoadSynchronous())
		{
			if (ProjectileMesh)
			{
				ProjectileMesh->SetSkeletalMesh(LoadedMesh);
			}
		}
	}

	if (ProjectileMesh && CollisionComp)
	{
		FBoxSphereBounds MeshBounds = ProjectileMesh->GetSkeletalMeshAsset()->GetBounds();
		FVector MeshSize = MeshBounds.BoxExtent;

		float AutoRadius = FMath::Max(MeshSize.X, MeshSize.Y);
		float AutoHalfHeight = MeshSize.Z;

		AutoRadius = FMath::Clamp(AutoRadius, 5.f, 200.f);
		AutoHalfHeight = FMath::Clamp(AutoHalfHeight, 5.f, 200.f);

		CollisionComp->SetCapsuleSize(AutoRadius, AutoHalfHeight, true);
	}

	GravityScale = ThrowableWeaponData->GravityScale;
	InitialSpeed = ThrowableWeaponData->InitialSpeed;
	MaxSpeed = ThrowableWeaponData->MaxSpeed;
	bRotationFollowsVelocity = ThrowableWeaponData->bRotationFollowsVelocity;
	Damage = ThrowableWeaponData->ProjectileDamage;
	LifeSpan = ThrowableWeaponData->ProjectileLifeSpan;

	if (ProjectileMovement)
	{
		ProjectileMovement->ProjectileGravityScale = GravityScale;
		ProjectileMovement->InitialSpeed = InitialSpeed;
		ProjectileMovement->MaxSpeed = MaxSpeed;
		ProjectileMovement->bRotationFollowsVelocity = bRotationFollowsVelocity;
	}
	SetLifeSpan(LifeSpan);
}