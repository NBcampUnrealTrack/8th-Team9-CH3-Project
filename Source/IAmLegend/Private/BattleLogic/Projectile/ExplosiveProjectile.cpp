// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleLogic/Projectile/ExplosiveProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "BattleLogic/Weapon/DataAssets/ThrowableWeaponDataAsset.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

AExplosiveProjectile::AExplosiveProjectile()
{
	// 초기값 설정
	ExplosionRadius = 500.f;	// 폭발 반경 기본값
	Damage = 100.f;			// 폭발 피해 기본값
	Friction = 0.6f;			// 마찰 계수 기본값
	bIsBouncing = true;		// 튕김 허용 여부 기본값
	Bounciness = 0.4f;		// 튕김 계수 기본값
	BounceVelocityThreshold = 50.f;	// 튕김이 발생하는 최소 속도 기본값

	ProjectileMovement->Bounciness = Bounciness;
	ProjectileMovement->Friction = Friction;
	ProjectileMovement->bShouldBounce = bIsBouncing;
	ProjectileMovement->BounceVelocityStopSimulatingThreshold = BounceVelocityThreshold;
}

void AExplosiveProjectile::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

}

void AExplosiveProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(EndPlayReason == EEndPlayReason::Destroyed || EndPlayReason == EEndPlayReason::RemovedFromWorld)
	{
		Explode();
	}

	Super::EndPlay(EndPlayReason);
}

void AExplosiveProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 이미 충돌한 경우 추가 충돌 처리 방지
	if (bHasHit)
	{
		return;
	}

	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);

	bHasHit = true;

}

void AExplosiveProjectile::Explode()
{
	// 폭발 처리 (피해 적용, 이펙트 재생 등)
	UGameplayStatics::ApplyRadialDamage(
		this, 
		Damage, 
		GetActorLocation(), 
		ExplosionRadius,
		nullptr, 
		TArray<AActor*>(), 
		this,
		GetInstigatorController(),
		false,	// 중심부에서 멀어질수록 데미지가 감소함
		ECC_Visibility	// 가려진 경우에는 데미지를 주지 않음
	);

	if (ExplosionFX)
	{
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ExplosionFX,
			GetActorLocation(),
			GetActorRotation()
		);

		if (NiagaraComp)
		{
			NiagaraComp->SetAutoDestroy(true);
		}
	}
}

void AExplosiveProjectile::InitProjectileFromData(UThrowableWeaponDataAsset* ThrowableWeaponData)
{
	Super::InitProjectileFromData(ThrowableWeaponData);

	if (!ThrowableWeaponData) return;

	ExplosionFX = ThrowableWeaponData->ExplosionEffect;
	ExplosionRadius = ThrowableWeaponData->ExplosionRadius;
	Friction = ThrowableWeaponData->Friction;
	bIsBouncing = ThrowableWeaponData->bIsBouncing;
	Bounciness = ThrowableWeaponData->Bounciness;
	BounceVelocityThreshold = ThrowableWeaponData->BounceVelocityThreshold;

	ProjectileMovement->Bounciness = Bounciness;
	ProjectileMovement->Friction = Friction;
	ProjectileMovement->bShouldBounce = bIsBouncing;
	ProjectileMovement->BounceVelocityStopSimulatingThreshold = BounceVelocityThreshold;

}