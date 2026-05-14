// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleLogic/WeaponBase.h"
#include "ThrowableWeaponBase.generated.h"

/**
 * 
 */
UCLASS()
class IAMLEGEND_API AThrowableWeaponBase : public AWeaponBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UTrajectoryComponent* TrajectoryComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Throwable")
	TSubclassOf<class AWeaponProjectileBase> ProjectileClass; // 투척 시 생성할 투사체 클래스
public:
	AThrowableWeaponBase();
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void StartWeaponAttack() override;		// 기본 공격 시작
	virtual void WeaponInitFromData() override;

	void ThrowWeapon();						// 무기 투척 (ProjectileClass를 사용하여 투사체 생성)

	void EnableTrajectory(bool bEnable);	// 투사체 궤적 표시 활성화/비활성화
};
