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

	// 공격 속도에 따라 공격이 끝나는 시점을 관리하기 위한 타이머 핸들
	// 추후 공격 애니메이션이 적용되면 애니메이션 노티파이로 대체할 수 있습니다.
	// 현재는 AttackDuration에 따라 타이머의 시간 간격을 조절하여 공격이 끝나는 시점을 관리할 수 있습니다.
	FTimerHandle AttackTimerHandle;

public:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Tick(float DeltaTime) override;
	virtual void WeaponAttack() override;		// 기본 공격
	virtual void SubAttack() override;		// 서브 공격 (투척)
	virtual void WeaponInitFromData() override;

	void EndAttack(); // 공격 종료 시 호출

	
};
