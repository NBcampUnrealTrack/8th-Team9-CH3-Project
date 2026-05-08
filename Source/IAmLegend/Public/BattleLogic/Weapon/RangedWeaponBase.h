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
	float FireRate; // 발사 속도 (분당 발사 횟수)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ranged")
	int32 CurrentAmmo; // 현재 탄약 수

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ranged")
	int32 MaxAmmo; // 최대 탄약 수
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ranged")
	float ReloadTime; // 재장전 시간 (초)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ranged")
	float AimingBonus;	// 조준 시 탄 퍼짐 감소량 (배율로 적용, 예: 0.5면 퍼짐이 절반으로 감소)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ranged")
	float RecoilAmount; // 반동량

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ranged|Spread")
	float SpreadPerShot; // 발사할 때마다 증가하는 탄 퍼짐량 (각도 단위)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ranged|Spread")
	float BaseSpreadAngle; // 기본 탄 퍼짐 각도

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ranged|Spread")
	float MaxSpreadAngle; // 최대 탄 퍼짐 각도 (발사할 때마다 증가하여 이 값을 넘지 않도록 함)

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Ranged|Spread")
	float CurrentSpreadAngle; // 현재 탄 퍼짐 각도 (발사할 때마다 증가)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ranged|Spread")
	float RecoverySpreadSpeed; // 탄 퍼짐 회복 속도 (초당 퍼짐 각도가 감소하는 양)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ranged")
	FName MuzzleSocketName; // 총구 소켓 이름
	
	bool bIsPressingAttack; // 공격 버튼이 눌려있는지 여부 (자동 발사 관리용)
	float FireInterval;		// 발사 간격 (초) - FireRate를 기반으로 계산
	float CoolDownTime;		// 발사 후 쿨다운 시간 (초) - FireRate를 기반으로 계산
	bool bIsReloading;		// 재장전 중인지 여부
	bool bIsCoolDown;		// 발사 쿨다운 중인지 여부

	FTimerHandle FireRateTimerHandle;		// 발사 속도 관리 타이머 핸들
	FTimerHandle ReloadTimerHandle;			// 재장전 관리 타이머 핸들
	FTimerHandle CoolDownTimerHandle;		// 발사 쿨다운 관리 타이머 핸들
	FTimerHandle SpreadRecoveryTimerHandle; // 탄 퍼짐 회복 관리 타이머 핸들

	virtual void BeginPlay() override;

public:
	ARangedWeaponBase();
	
	virtual void StartWeaponAttack() override;		// 기본 공격 (발사)
	virtual void StopWeaponAttack() override;		// 기본 공격 종료 (발사 중지)
	virtual void WeaponInitFromData() override;	// 데이터 에셋에서 초기화

	void Reload();			// 재장전
	void FinishReload();	// 재장전 완료 시 호출
	void HandleFire();		// 실제 발사 처리 (발사 속도에 따라 타이머로 호출)
	void ApplyRecoil();		// 반동 적용
	void FinishCooldown();	// 발사 쿨다운 종료 시 호출
	void ApplySpread(bool bIsAiming);		// 탄 퍼짐 증가
	void RecoverSpread();	// 탄 퍼짐 회복

	virtual void Fire();		// 발사 시 탄이 날아가는 경로를 계산하고 타격 판정 처리
	bool CanFire() const;		// 발사 가능한지 여부 체크 (탄약, 재장전 상태 등)

};
