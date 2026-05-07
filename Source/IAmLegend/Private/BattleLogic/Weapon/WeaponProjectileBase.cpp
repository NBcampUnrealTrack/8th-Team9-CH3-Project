// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleLogic/Weapon/WeaponProjectileBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AWeaponProjectileBase::AWeaponProjectileBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("SphereComp"));
	CollisionComp->InitCapsuleSize(5.f, 50.f);
	CollisionComp->SetCollisionProfileName(TEXT("Projectile"));

	CollisionComp->SetNotifyRigidBodyCollision(true);
	CollisionComp->OnComponentHit.AddDynamic(this, &AWeaponProjectileBase::OnHit);
	RootComponent = CollisionComp;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	InitialSpeed = 3000.f;
	MaxSpeed = 3000.f;
	bRotationFollowsVelocity = true;
	bShouldBounce = false;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = InitialSpeed;
	ProjectileMovement->MaxSpeed = MaxSpeed;
	ProjectileMovement->bRotationFollowsVelocity = bRotationFollowsVelocity;
	ProjectileMovement->bShouldBounce = bShouldBounce;

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

void AWeaponProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("Projectile hit: %s"), *OtherActor->GetName());
	// 이미 충돌한 경우 추가 충돌 처리 방지
	if (bHasHit)
	{
		return; 
	}
	bHasHit = true;

	if (OtherActor && (OtherActor != this) && (OtherActor != GetInstigator()) && (OtherComp != nullptr))
	{		
		UGameplayStatics::ApplyDamage(OtherActor, Damage, GetOwner()->GetInstigatorController(), GetOwner(), nullptr);
	}
}

