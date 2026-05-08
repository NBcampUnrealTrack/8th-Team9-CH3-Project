// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponItemActor.h"
#include "WeaponBase.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Melee,      // 근접 무기
	Ranged,     // 원거리 무기
	Throwable   // 투척 무기
};

UCLASS()
class IAMLEGEND_API AWeaponBase : public AWeaponItemActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	float Damage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	float Range;

	UPROPERTY()
	TArray<AActor*> HitActors;	// 공격 시 이미 타격한 액터들을 저장

	// 현재는 더미를 기준으로 코드를 작성하였습니다.
	// 추후에 실제 플레이어 캐릭터 클래스로 변경해야 합니다.
	// 다른 무기 클래스에도 HanPlayerCharacter.h를 미리 include 해두었습니다.
	// WeaponBase.h와 WeaponBase.cpp에서 OwnerCharacter를 사용하는 부분만 실제 플레이어 캐릭터 클래스로 변경하면 됩니다.
	class ADummyPlayerCharacter* OwnerCharacter;

public:
	AWeaponBase();

	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void StartWeaponAttack();	// 기본 공격
	virtual void StopWeaponAttack();	// 공격 종료 (예: 근접 무기 휘두르기 끝, 원거리 무기 발사 후 등)

	virtual void WeaponInitFromData();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void DestroyWeapon();			// 무기 파괴 (예: 투척 후)
	
};
