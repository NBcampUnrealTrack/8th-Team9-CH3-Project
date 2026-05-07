// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleLogic/WeaponBase.h"
#include "RangedWeaponBase.generated.h"

/**
 * 
 */
UCLASS()
class IAMLEGEND_API ARangedWeaponBase : public AWeaponBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ranged")
	float FireRate; // 발사 속도 (초당 발사 횟수)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ranged")
	int32 CurrentAmmo; // 현재 탄약 수

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ranged")
	int32 MaxAmmo; // 최대 탄약 수

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ranged")
	float RecoilAmount; // 반동량

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ranged")
	float SpreadAngle; // 탄 퍼짐 각도

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ranged")
	FName MuzzleSocketName; // 총구 소켓 이름

	bool bIsCanFire; // 발사 가능한 상태인지 여부
	bool bIsReloading; // 재장전 중인지 여부
	FTimerHandle FireRateTimerHandle; // 발사 속도 관리 타이머 핸들
	FTimerHandle ReloadTimerHandle; // 재장전 관리 타이머 핸들

public:
	/*
	ARangedWeaponBase();

	virtual void StartWeaponAttack() override;		// 기본 공격 (발사)
	virtual void StopWeaponAttack() override;		// 기본 공격 종료 (발사 중지)
	virtual void StartSubAttack() override;		// 서브 공격 (조준 사격)
	virtual void StopSubAttack() override;		// 서브 공격 종료 (조준 사격 중지)

	virtual void StartFire();		// 발사 시작
	virtual void StopFire();		// 발사 중지
	virtual void Reload();			// 재장전
	virtual void FinishReload();	// 재장전 완료 시 호출
	virtual void HandleFire();		// 실제 발사 처리 (발사 속도에 따라 타이머로 호출)
	virtual void ApplyRecoil();		// 반동 적용

	virtual void Fire();		// 발사 시 탄이 날아가는 경로를 계산하고 타격 판정 처리
	*/

};
