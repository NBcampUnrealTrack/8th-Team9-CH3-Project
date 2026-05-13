// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleLogic/Weapon/TwoHandRangedWeaponBase.h"

ATwoHandRangedWeaponBase::ATwoHandRangedWeaponBase()
{
	// 초기값 설정 (추후에 WeaponDataAsset에서 초기화 하는 것으로 변경 예정입니다.)
	WeaponType = EWeaponType::TwoHandedRanged; // 무기 타입 설정
}