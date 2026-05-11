// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleLogic/Projectile/WeaponProjectileBase.h"
#include "BluntProjectile.generated.h"

/**
 * 
 */
UCLASS()
class IAMLEGEND_API ABluntProjectile : public AWeaponProjectileBase
{
	GENERATED_BODY()

public:
	ABluntProjectile();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Damping")
	float LinearDamping;	// 이동 저항 (0.0f 이상, 값이 클수록 빨리 멈춥니다)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Damping")
	float AngularDamping;	// 회전 저항 (0.0f 이상, 값이 클수록 빨리 멈춥니다)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Damping")
	float ImpactDamping;	// 충돌 시 속도 감쇠 계수 (0.0f ~ 1.0f, 0이면 완전히 멈추고 1이면 감쇠 없음)

	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	
};
