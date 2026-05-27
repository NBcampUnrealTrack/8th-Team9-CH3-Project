// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleLogic/Weapon/MeleeWeaponBase.h"
#include "WeaponDataAsset.h"
#include "Kismet/GameplayStatics.h"
#include "BattleLogic/Projectile/WeaponProjectileBase.h"
#include "Character/HanPlayerCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BattleLogic/Weapon/DataAssets/MeleeWeaponDataAsset.h"

#define ATTACK_TRACE_CHANNEL ECC_GameTraceChannel1

AMeleeWeaponBase::AMeleeWeaponBase()
{
	// 기본 값 설정
	WeaponType = EWeaponType::TwoHandedMelee; // 무기 타입 설정
	WeaponSlot = EWeaponSlot::Melee; // 무기 슬롯 설정
	AttackCooldown = .5f; // 공격 간격
	AttackSpeedRate = 1.0f; // 공격 속도 배율 (기본값은 1.0)

	bIsAttacking = false; // 공격 중인지 여부
	bIsPressingAttack = false; // 공격 버튼이 눌려있는지 여부 (자동 공격 관리용)
	bIsCooldown = false; // 공격 쿨다운 중인지 여부
}

void AMeleeWeaponBase::BeginPlay()
{
	Super::BeginPlay();

}

void AMeleeWeaponBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AMeleeWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!SkeletalMesh || !OwnerCharacter) return;

	if (bIsAttacking)
	{
		AttackTrace(); // 공격 판정 수행
	}
}

void AMeleeWeaponBase::StartWeaponAttack()
{
	bIsPressingAttack = true; // 공격 버튼이 눌려있는 상태로 설정

	// 이미 공격 중이거나 쿨다운 중이거나 소유자 캐릭터가 없는 경우라면 공격 방지
	if (bIsAttacking || bIsCooldown || !OwnerCharacter)
	{
		return; 
	}

	// 조준 중이라면 부모 클래스인 ThrowableWeaponBase의 StartWeaponAttack을 내부에서 ThrowWeapon()를 호출하여 투척 공격을 실행합니다.
	Super::StartWeaponAttack();

	// 조준 중이 아니라면 공격 실행
	if (!(OwnerCharacter->IsAiming()))
	{
		ExcuteAttack(); // 실제 공격 처리 (현재는 타이머로 호출하지만, 추후에 공격 애니메이션이 적용되면 애니메이션 노티파이로 대체할 수 있습니다.)
	}
}

void AMeleeWeaponBase::EndAttack()
{
	bIsAttacking = false;		// 공격 종료 상태로 설정
	bIsCooldown = true;			// 쿨다운 상태로 설정
	SetActorTickEnabled(false);	// 틱을 비활성화해 타격 판정 중지
	HitActors.Empty();			// 타격한 액터 목록 초기화	

	// 쿨타임이 끝나는 시점을 관리하기 위해 타이머 설정
	GetWorldTimerManager().SetTimer(
		AttackIntervalTimerHandle,
		this,
		&AMeleeWeaponBase::FinishCooldown,
		AttackCooldown,
		false
	);

	UE_LOG(LogTemp, Warning, TEXT("Melee Attack Ended!"));
}

void AMeleeWeaponBase::StopWeaponAttack()
{
	Super::StopWeaponAttack();
	bIsPressingAttack = false; // 공격 버튼이 눌려있지 않은 상태로 설정
}

void AMeleeWeaponBase::ExcuteAttack()
{
	// 공격이 시작될 때 공격 속도에 따라 공격 애니메이션 재생
	OwnerCharacter->PlayAttackMontage_1(AttackSpeedRate);

	// 공격이 시작될 때 타이머를 초기화
	GetWorldTimerManager().ClearTimer(AttackIntervalTimerHandle);

	bIsAttacking = true;		// 공격 중 상태로 설정
	SetActorTickEnabled(true);	// 틱을 활성화해 타격 판정 시작
}

void AMeleeWeaponBase::AttackTrace()
{
	// 날의 시작과 끝 위치
	FVector Start = SkeletalMesh->GetSocketLocation(FName("Root"));
	FVector End = SkeletalMesh->GetSocketLocation(FName("Tip"));

	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter); // 플레이어는 충돌에서 제외
	Params.AddIgnoredActor(this);		// 자신은 충돌에서 제외

	float WeaponThickness = 10.0f; // 칼날의 가상 두께

	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		ATTACK_TRACE_CHANNEL,
		FCollisionShape::MakeSphere(WeaponThickness),
		Params
	);

	ProcessHitResults(HitResults); // 타격 결과 처리 (데미지 적용 등)
}

void AMeleeWeaponBase::ProcessHitResults(const TArray<FHitResult>& HitResults)
{
	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && !HitActors.Contains(HitActor))
		{
			// 타격한 액터에 데미지를 적용
			UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitActor->GetName());
			UGameplayStatics::ApplyDamage(HitActor, Damage, OwnerCharacter->GetInstigatorController(), OwnerCharacter, nullptr);
			HitActors.Add(HitActor); // 이미 타격한 액터를 추가하여 중복 타격 방지
		}
	}
}

void AMeleeWeaponBase::FinishCooldown()
{
	bIsCooldown = false; // 쿨다운 종료
	if (bIsPressingAttack) // 공격 버튼이 여전히 눌려있다면 자동으로 다음 공격 시작
	{
		StartWeaponAttack();
	}
}

void AMeleeWeaponBase::AnimNotify_EndAttack_1()
{
	Super::AnimNotify_EndAttack_1();
	EndAttack(); // 공격 종료 처리
	
}

void AMeleeWeaponBase::WeaponInitFromData()
{
	Super::WeaponInitFromData();

	if (!ItemData) return;

	if (UMeleeWeaponDataAsset* MeleeWeaponData = Cast<UMeleeWeaponDataAsset>(ItemData))
	{
		AttackCooldown = MeleeWeaponData->AttackCooldown;
		AttackSpeedRate = MeleeWeaponData->AttackSpeedRate;
	}
}
