// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleLogic/Projectile/WeaponProjectileBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AWeaponProjectileBase::AWeaponProjectileBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("SphereComp"));
	CollisionRadius = 5.f;
	CollisionHalfHeight = 50.f;
	CollisionComp->SetCapsuleSize(CollisionRadius, CollisionHalfHeight);
	CollisionComp->SetCollisionProfileName(TEXT("Projectile"));
	CollisionComp->SetNotifyRigidBodyCollision(true);
	CollisionComp->OnComponentHit.AddDynamic(this, &AWeaponProjectileBase::OnHit);
	RootComponent = CollisionComp;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
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

	SetLifeSpan(LifeSpan);
}

// Called when the game starts or when spawned
void AWeaponProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeaponProjectileBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	CollisionComp->SetCapsuleSize(CollisionRadius, CollisionHalfHeight);
}

void AWeaponProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("Projectile hit: %s"), *OtherActor->GetName());
}

