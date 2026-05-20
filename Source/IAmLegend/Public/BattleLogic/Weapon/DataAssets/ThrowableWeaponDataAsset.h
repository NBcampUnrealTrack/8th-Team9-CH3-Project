// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponDataAsset.h"
#include "ThrowableWeaponDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class IAMLEGEND_API UThrowableWeaponDataAsset : public UWeaponDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Throwable")
	TSubclassOf<class AWeaponProjectileBase> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Throwable|Projectile|Visual")
	TSoftObjectPtr<class UNiagaraSystem> TrajectorySystemAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Throwable|Projectile|Stats")
	float ProjectileDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Throwable|Projectile|Stats")
	float ProjectileLifeSpan;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Throwable|Projectile|Movement")
	float InitialSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Throwable|Projectile|Movement")
	float MaxSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Throwable|Projectile|Movement")
	float GravityScale;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Throwable|Projectile|Movement")
	bool bRotationFollowsVelocity;

	// 폭발형 데이터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Throwable|Projectile|Explosive")
	float ExplosionRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Throwable|Projectile|Explosive")
	float Bounciness;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Throwable|Projectile|Explosive")
	float BounceVelocityThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Throwable|Projectile|Explosive")
	float Friction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Throwable|Projectile|Explosive")
	bool bIsBouncing;

	// 둔기형 데이터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Throwable|Projectile|Blunt")
	float LinearDamping;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Throwable|Projectile|Blunt")
	float AngularDamping;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Throwable|Projectile|Blunt")
	float ImpactDamping;
};
