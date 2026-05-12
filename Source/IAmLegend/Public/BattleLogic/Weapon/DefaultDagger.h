// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleLogic/Weapon/MeleeWeaponBase.h"
#include "DefaultDagger.generated.h"

/**
 * 
 */
UCLASS()
class IAMLEGEND_API ADefaultDagger : public AMeleeWeaponBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Dagger")
	float StabDamage; // 찌르기 공격의 데미지

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Dagger")
	float StabCooldown; // 찌르기 공격의 쿨다운 시간

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Dagger")
	float StabDuration; // 찌르기 공격의 지속 시간
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Dagger")
	float StabRange; // 찌르기 공격의 최대 사거리

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Dagger")
	FVector StabBoxExtent; // 찌르기 공격의 범위 (박스 형태)

	bool bIsStabbing; // 찌르기 공격 중인지 여부

public:
	ADefaultDagger();

	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Tick(float DeltaTime) override;
	virtual void StartWeaponAttack() override;		

	virtual void WeaponInitFromData() override;

	void ExcuteStab();
	void EndStab();
	void StabTrace();	// 찌르기 공격 판정 수행(범위 기준)
	void StabTraceMesh(); // 찌르기 공격 판정 수행(메쉬 기준)
};
