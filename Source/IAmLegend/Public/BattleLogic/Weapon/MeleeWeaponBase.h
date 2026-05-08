// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleLogic/WeaponBase.h"
#include "MeleeWeaponBase.generated.h"

/**
 * 
 */
UCLASS()
class IAMLEGEND_API AMeleeWeaponBase : public AWeaponBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Melee")
	float SwingSpeed; // 휘두르는 속도

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Melee")
	float AttackDuration; // 공격 지속 시간

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Melee")
	bool bIsAttacking; // 공격 중인지 여부

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Melee")
	bool bIsCooldown; // 공격 쿨다운 중인지 여부

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Melee")
	float AttackCooldown; // 공격 간격 (초)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Throwable")
	TSubclassOf<class AWeaponProjectileBase> ProjectileClass; // 투척 시 생성할 투사체 클래스

	// 공격 속도에 따라 공격이 끝나는 시점을 관리하기 위한 타이머 핸들
	// 추후 공격 애니메이션이 적용되면 애니메이션 노티파이로 대체할 수 있습니다.
	// 현재는 AttackDuration에 따라 타이머의 시간 간격을 조절하여 공격이 끝나는 시점을 관리할 수 있습니다.
	FTimerHandle AttackTimerHandle;
	FTimerHandle AttackIntervalTimerHandle; // 자동 공격 관리 타이머 핸들
	bool bIsPressingAttack; // 공격 버튼이 눌려있는지 여부 (자동 공격 관리용)

public:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Tick(float DeltaTime) override;
	virtual void StartWeaponAttack() override;		// 기본 공격 시작
	virtual void StopWeaponAttack() override;		// 기본 공격 종료
	virtual void StartSubAttack() override;			// 서브 공격 (투척)
	virtual void StopSubAttack() override;			// 서브 공격 종료
	
	virtual void WeaponInitFromData() override;

	void ExcuteAttack();						// 실제 공격 처리 (타이머로 호출)
	void EndAttack(); // 공격 종료 시 호출
	void FinishCooldown();					// 공격 쿨다운 종료 시 호출
	
};
