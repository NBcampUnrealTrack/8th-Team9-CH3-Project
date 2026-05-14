// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleLogic/Projectile/WeaponProjectileBase.h"
#include "ExplosiveProjectile.generated.h"

/**
 * 
 */
UCLASS()
class IAMLEGEND_API AExplosiveProjectile : public AWeaponProjectileBase
{
	GENERATED_BODY()
	
public:
	AExplosiveProjectile();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	float ExplosionRadius;	// 폭발 반경

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Friction;			// 마찰 계수

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Bouncing")
	bool bIsBouncing;	// 투사체가 튕기는 중인지 여부

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Bouncing")
	float Bounciness;	// 튕김 계수 (0.0f ~ 1.0f, 0이면 튕기지 않고 1이면 완전히 튕깁니다)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Bouncing")
	float BounceVelocityThreshold;	// 튕김이 발생하는 최소 속도 (이 값보다 낮으면 튕기지 않고 멈춥니다)

	void OnConstruction(const FTransform& Transform) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	void Explode();	// 폭발 처리 (피해 적용, 이펙트 재생 등)
};
