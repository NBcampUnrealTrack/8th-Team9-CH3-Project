// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleLogic/Weapon/TwoHandRangedWeaponBase.h"
#include "ShotgunBase.generated.h"

/**
 * 
 */
UCLASS()
class IAMLEGEND_API AShotgunBase : public ATwoHandRangedWeaponBase
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Shotgun")
	int32 PelletsPerShot;	// 샷건 발당 발사되는 펠릿 수
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Shotgun")
	int32 AmmoPerReload;		// 재장전 시 장전되는 탄약 수 (탄창을 사용하는 샷건을 고려해 추가했습니다.)

public:
	AShotgunBase();

	void BeginPlay() override;

	void StartWeaponAttack() override;		// 기본 공격 (발사)
	void FinishReload() override;			// 재장전 완료 시 호출
	void Fire() override;					// 발사

	virtual void WeaponInitFromData() override;	// 데이터 에셋에서 초기화

};
