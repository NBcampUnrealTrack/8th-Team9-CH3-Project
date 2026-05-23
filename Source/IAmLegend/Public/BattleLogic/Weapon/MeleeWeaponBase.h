// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleLogic/WeaponBase.h"
#include "BattleLogic/Weapon/ThrowableWeaponBase.h"
#include "MeleeWeaponBase.generated.h"

/**
 * 
 */
UCLASS()
class IAMLEGEND_API AMeleeWeaponBase : public AThrowableWeaponBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Melee")
	bool bIsAttacking; // 공격 중인지 여부

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Melee")
	bool bIsCooldown; // 공격 쿨다운 중인지 여부

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Melee")
	float AttackCooldown; // 공격 간격 (초)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Melee")
	float AttackSpeedRate; // 공격 속도 배율 (1.0 = 기본 속도, 2.0 = 2배 빠름 등)

	FTimerHandle AttackIntervalTimerHandle; // 자동 공격 관리 타이머 핸들
	bool bIsPressingAttack; // 공격 버튼이 눌려있는지 여부 (자동 공격 관리용)

public:
	AMeleeWeaponBase();

	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Tick(float DeltaTime) override;
	virtual void StartWeaponAttack() override;		// 기본 공격 시작
	virtual void StopWeaponAttack() override;		// 기본 공격 종료
	
	virtual void WeaponInitFromData() override;

	void ExcuteAttack();						// 실제 공격 처리 (타이머로 호출)
	void EndAttack(); // 공격 종료 시 호출
	void AttackTrace();						// 공격 범위에 대한 충돌 검사 수행
	void ProcessHitResults(const TArray<FHitResult>& HitResults); // 공격 결과 처리 (타격한 액터에 데미지 적용 등)
	void FinishCooldown();					// 공격 쿨다운 종료 시 호출

	// 애니메이션 노티파이
	virtual void AnimNotify_EndAttack_1() override;	// 일반 공격 애니메이션 종료 시 호출 (애니메이션 노티파이로 설정)	
	//virtual void AnimNotify_EndAttack_2();	// 조준 공격 애니메이션 종료 시 호출 (애니메이션 노티파이로 설정)
	//virtual void AnimNotify_EndReload() override;	// 재장전 애니메이션 종료 시 호출 (애니메이션 노티파이로 설정)
	
};
