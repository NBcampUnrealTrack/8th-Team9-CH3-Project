// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleLogic/Weapon/RangedWeaponBase.h"
#include "PistolBase.generated.h"

/**
 * 
 */
UCLASS()
class IAMLEGEND_API APistolBase : public ARangedWeaponBase
{
	GENERATED_BODY()
protected:
	/* 근접 공격 처리시 메시를 필요로 할경우 해당 메시 컴포넌트를 추가해야 합니다.
	* 현재는 범위를 기준으로 트레이스하기에 주석처리 해두었습니다.
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> DaggerMesh;	// 근접 공격용 메시
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Pistol|Melee")
	TObjectPtr<USkeletalMeshComponent> DaggerMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Pistol|Melee")
	FName DaggerSocketName; // 근접 공격 시작 위치 소켓 이름

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Pistol|Melee")
	float MeleeDamage;	// 근접 공격 데미지

public:
	APistolBase();

	void BeginPlay() override;

	void MeleeAttackTrace() override;	// 근접 공격 트레이스 계산 및 타격 판정
	void ProcessMeleeHits(const TArray<FHitResult>& HitResults) override;	// 근접 공격 타격 처리

	virtual void WeaponInitFromData() override;	// 데이터 에셋에서 초기화

};
