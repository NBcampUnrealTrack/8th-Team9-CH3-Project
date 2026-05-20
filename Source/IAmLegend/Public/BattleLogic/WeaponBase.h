// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponItemActor.h"
#include "WeaponBase.generated.h"

// 무기 타입이 추가로 필요하다면 여기에 추가하시면 됩니다.
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Dagger,     // 단검
	Pistol,     // 권총
	Rifle,      // 소총
	Shotgun,    // 산탄총
	Granade,    // 수류탄

	// 추가 무기 타입 예시
	OneHandedMelee,	// 한손 근접 무기
	TwoHandedMelee,	// 양손 근접 무기
	OneHandedRanged,	// 한손 원거리 무기
	TwoHandedRanged,	// 양손 원거리 무기
};

// 무기 슬롯
UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	Melee       UMETA(DisplayName = "Melee"),   // 근접 무기 (1번)
	Ranged       UMETA(DisplayName = "Ranged"), // 총 (2번)
	Dagger      UMETA(DisplayName = "Dagger"),  // 단검 (3번)
	Grenade     UMETA(DisplayName = "Grenade"), // 수류탄 (4번)
	None
};

UCLASS()
class IAMLEGEND_API AWeaponBase : public AWeaponItemActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<USkeletalMeshComponent> SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	float Damage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	float Range;

	UPROPERTY()
	TArray<AActor*> HitActors;	// 공격 시 이미 타격한 액터들을 저장

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	EWeaponType WeaponType;		// 무기 타입 (근접, 원거리, 투척 등)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	EWeaponSlot WeaponSlot;		// 무기 슬롯 (근접, 원거리, 단검, 수류탄 등)

	class AHanPlayerCharacter* OwnerCharacter;

public:
	AWeaponBase();

	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void StartWeaponAttack();	// 기본 공격
	virtual void StopWeaponAttack();	// 공격 종료 (예: 근접 무기 휘두르기 끝, 원거리 무기 발사 후 등)

	virtual void WeaponInitFromData();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	EWeaponType GetWeaponType() const;		// 무기 타입 반환 (근접, 원거리, 투척 등)

	EWeaponSlot GetWeaponSlot() const;		// 무기 슬롯 반환 (근접, 원거리, 단검, 수류탄 등)

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void DestroyWeapon();			// 무기 파괴 (예: 투척 후)
	
	// 애니메이션 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon | Animation")
	class UAnimMontage* Attack_1_Montage = nullptr; // 일반 공격 (좌클릭)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon | Animation")
	class UAnimMontage* Attack_2_Montage = nullptr; // 조준 공격 (우클릭 도중 좌클릭)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon | Animation")
	class UAnimMontage* Reload_Montage = nullptr;   // 재장전 몽타주

	// 애니메이션 노티파이
	UFUNCTION(BlueprintCallable, Category = "Weapon|Animation")
	virtual void AnimNotify_EndAttack_1();	// 일반 공격 애니메이션 종료 시 호출 (애니메이션 노티파이로 설정)

	UFUNCTION(BlueprintCallable, Category = "Weapon|Animation")
	virtual void AnimNotify_EndAttack_2();	// 조준 공격 애니메이션 종료 시 호출 (애니메이션 노티파이로 설정)

	UFUNCTION(BlueprintCallable, Category = "Weapon|Animation")
	virtual void AnimNotify_EndReload();	// 재장전 애니메이션 종료 시 호출 (애니메이션 노티파이로 설정)

	// 무기 UI 아이콘 - 김민성
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	class UTexture2D* WeaponIcon;
};
