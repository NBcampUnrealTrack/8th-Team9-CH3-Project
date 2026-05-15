// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleLogic/WeaponBase.h"
#include "ThrowableWeaponBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponThrown);

/**
 * 
 */
UCLASS()
class IAMLEGEND_API AThrowableWeaponBase : public AWeaponBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Throwable")
	TSubclassOf<class AWeaponProjectileBase> ProjectileClass; // 투척 시 생성할 투사체 클래스
public:
	UPROPERTY(BlueprintAssignable, Category = "Weapon|Event")
	FOnWeaponThrown OnWeaponThrown;

	AThrowableWeaponBase();
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void StartWeaponAttack() override;		// 기본 공격 시작
	virtual void ThrowWeapon();						// 무기 투척 (ProjectileClass를 사용하여 투사체 생성)
	virtual void WeaponInitFromData() override;
};
