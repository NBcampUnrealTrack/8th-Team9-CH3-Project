// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleLogic/Projectile/SharpProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/CapsuleComponent.h"

ASharpProjectile::ASharpProjectile()
{
	// 초기값 설정
}

void ASharpProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 이미 충돌한 경우 추가 충돌 처리 방지
	if (bHasHit)
	{
		return;
	}

	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);

	bHasHit = true;

	// 충돌 시 투사체의 움직임을 멈추고 비활성화
	ProjectileMovement->StopMovementImmediately();
	ProjectileMovement->Deactivate();

	// 충돌 후 물리 시뮬레이션 비활성화
	CollisionComp->SetSimulatePhysics(false);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 충돌한 액터에 투사체를 붙임
	AttachToHitActor(OtherActor, OtherComp, Hit);
}

void ASharpProjectile::AttachToHitActor(AActor* OtherActor, UPrimitiveComponent* OtherComp, const FHitResult& Hit)
{
	USkeletalMeshComponent* TargetMesh = OtherActor->FindComponentByClass<USkeletalMeshComponent>();

	if (ProjectileMesh && ProjectileMesh->DoesSocketExist(TEXT("Tip")))
	{
		// 투사체의 Tip 소켓 위치를 충돌 지점에 맞춰 조정
		FVector TipWorldLoc = ProjectileMesh->GetSocketLocation(TEXT("Tip"));
		FVector ActorWorldLoc = GetActorLocation();
		FVector Offset = TipWorldLoc - ActorWorldLoc;
		FVector FinalLocation = Hit.ImpactPoint - Offset + (GetActorForwardVector() * 10.f);
		SetActorLocationAndRotation(FinalLocation, GetActorRotation());

		// 투사체의 위치를 새로운 변환으로 설정
		SetActorTransform(FTransform(GetActorRotation(), FinalLocation));
	}

	// 액터가 스켈레톤 메시를 갖고있으면
	if (TargetMesh)
	{
		// 가장 가까운 본을 찾아서 그 본에 붙임
		FName Bone = TargetMesh->FindClosestBone(Hit.Location);

		AttachToComponent(TargetMesh, FAttachmentTransformRules::KeepWorldTransform, Bone);
		UE_LOG(LogTemp, Warning, TEXT("Bone Found: %s"), *Bone.ToString());
	}
	else
	{
		AttachToActor(OtherActor, FAttachmentTransformRules::KeepWorldTransform);
	}

	if (OtherActor && (OtherActor != this) && (OtherActor != GetInstigator()) && (OtherComp != nullptr))
	{
		UGameplayStatics::ApplyDamage(OtherActor, Damage, GetOwner()->GetInstigatorController(), GetOwner(), nullptr);
	}
}

void ASharpProjectile::InitProjectileFromData(UThrowableWeaponDataAsset* ThrowableWeaponData)
{
	Super::InitProjectileFromData(ThrowableWeaponData);
}