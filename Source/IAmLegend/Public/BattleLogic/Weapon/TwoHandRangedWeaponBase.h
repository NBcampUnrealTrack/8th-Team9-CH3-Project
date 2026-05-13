// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleLogic/Weapon/RangedWeaponBase.h"
#include "TwoHandRangedWeaponBase.generated.h"

/**
 * 
 */
UCLASS()
class IAMLEGEND_API ATwoHandRangedWeaponBase : public ARangedWeaponBase
{
	GENERATED_BODY()
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|TwoHandRanged")
	FString LeftHandIKSocketName;	// 왼손 IK 소켓 이름 "LeftHandIKSocket"

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Melee")
	float PushForce;	// 밀어내는 힘 (근접 공격 시 적을 밀어내는 효과를 위해)

public:
	ATwoHandRangedWeaponBase();

	void BeginPlay() override;

	void MeleeAttackTrace() override;	// 근접 공격 트레이스 계산 및 타격 판정
	void ProcessMeleeHits(const TArray<FHitResult>& HitResults) override;	// 근접 공격 타격 처리
};
