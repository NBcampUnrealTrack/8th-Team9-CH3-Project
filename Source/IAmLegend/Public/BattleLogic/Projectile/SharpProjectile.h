// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleLogic/Projectile/WeaponProjectileBase.h"
#include "SharpProjectile.generated.h"

/**
 * 
 */
UCLASS()
class IAMLEGEND_API ASharpProjectile : public AWeaponProjectileBase
{
	GENERATED_BODY()

public:
	ASharpProjectile();

protected:
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	void AttachToHitActor(AActor* OtherActor, UPrimitiveComponent* OtherComp, const FHitResult& Hit);
};
