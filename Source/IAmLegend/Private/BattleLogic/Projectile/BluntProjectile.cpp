// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleLogic/Projectile/BluntProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/CapsuleComponent.h"

ABluntProjectile::ABluntProjectile()
{
	LinearDamping = 1.0f;	// 이동 저항 기본값
	AngularDamping = 2.0f;	// 회전 저항 기본값
	ImpactDamping = 0.2f;	// 충돌 시 속도 감쇠 계수 기본값

	CollisionComp->SetLinearDamping(LinearDamping);
	CollisionComp->SetAngularDamping(AngularDamping);

}

void ABluntProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 이미 충돌한 경우 추가 충돌 처리 방지
	if (bHasHit)
	{
		return;
	}

	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);

	bHasHit = true;
	
	// 충돌 시 속도 감쇠 적용
	FVector DampedVelocity = ProjectileMovement->Velocity * ImpactDamping;

	// 충돌 시 투사체의 움직임을 멈추고 비활성화
	ProjectileMovement->StopMovementImmediately();
	ProjectileMovement->Deactivate();

	// 충돌 후 투사체가 자연스럽게 떨어지도록 물리 시뮬레이션 활성화
	CollisionComp->SetSimulatePhysics(true);

	// 감쇠된 속도로 투사체가 떨어지도록 설정
	CollisionComp->SetPhysicsLinearVelocity(DampedVelocity);

	if (OtherActor && (OtherActor != this) && (OtherActor != GetInstigator()) && (OtherComp != nullptr))
	{
		UGameplayStatics::ApplyDamage(OtherActor, Damage, GetOwner()->GetInstigatorController(), GetOwner(), nullptr);
	}

}