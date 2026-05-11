// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleLogic/Weapon/MeleeWeaponBase.h"
#include "WeaponDataAsset.h"
#include "Kismet/GameplayStatics.h"
#include "BattleLogic/Weapon/WeaponProjectileBase.h"
#include "Character/HanPlayerCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

#define ATTACK_TRACE_CHANNEL ECC_GameTraceChannel1

AMeleeWeaponBase::AMeleeWeaponBase()
{
	// 초기값 설정 (추후에 WeaponDataAsset에서 초기화 하는 것으로 변경 예정입니다.)
	SwingSpeed = 1.f; // 휘두르는 속도
	AttackDuration = 1.f; // 공격 지속 시간
	AttackCooldown = .5f; // 공격 간격
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

	if (!Mesh || !OwnerCharacter) return;

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
	// 현재는 타이머 기반으로 동작하지만, 추후에 공격 애니메이션이 적용되면 애니메이션 노티파이로 대체할 수 있습니다.
	// 공격이 시작될 때 타이머를 초기화
	GetWorldTimerManager().ClearTimer(AttackTimerHandle);
	GetWorldTimerManager().ClearTimer(AttackIntervalTimerHandle);

	bIsAttacking = true;		// 공격 중 상태로 설정
	SetActorTickEnabled(true);	// 틱을 활성화해 타격 판정 시작

	// 공격 판정이 끝나는 시점을 관리하기 위해 타이머 설정
	GetWorldTimerManager().SetTimer(
		AttackTimerHandle,
		this,
		&AMeleeWeaponBase::EndAttack,
		AttackDuration,
		false
	);
}

void AMeleeWeaponBase::AttackTrace()
{
	// 공격 범위에 대한 충돌 검사 수행 (현재는 Tick에서 처리하지만, 추후에 공격 애니메이션이 적용되면 애니메이션 노티파이로 대체할 수 있습니다.)
	// 날의 시작과 끝 위치
	FVector Start = Mesh->GetSocketLocation(FName("Root"));
	FVector End = Mesh->GetSocketLocation(FName("Tip"));

	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter); // 플레이어는 충돌에서 제외
	Params.AddIgnoredActor(this);		// 자신은 충돌에서 제외

	float WeaponThickness = 10.0f; // 칼날의 가상 두께
	/*bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		ATTACK_TRACE_CHANNEL,
		FCollisionShape::MakeSphere(WeaponThickness),
		Params
	);*/

	// 디버그 용
	bool bHit = UKismetSystemLibrary::SphereTraceMulti(
		GetWorld(),
		Start,                          // 시작 위치
		End,                            // 끝 위치
		WeaponThickness,                // 구체 반지름
		UEngineTypes::ConvertToTraceType(ATTACK_TRACE_CHANNEL), // 채널
		false,                          // 복합 콜리전 사용 여부
		{ OwnerCharacter, this },			// 충돌에서 제외할 액터 배열
		EDrawDebugTrace::ForDuration,   // 디버그 형태 (None, ForOneFrame, ForDuration)
		HitResults,                     // 결과 배열
		true,                           // 자기 자신 무시 여부
		FLinearColor::Red,              // 추적 선 색상
		FLinearColor::Green,            // 히트 시 색상
		1.0f                            // 디버그 선 유지 시간 (초)
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


void AMeleeWeaponBase::WeaponInitFromData()
{
	Super::WeaponInitFromData();

	if (UWeaponDataAsset* WeaponData = Cast<UWeaponDataAsset>(ItemData))
	{	
		/* 현재 WeaponDataAsset에는 Damage만 작성되어 있어 SwingSpeed 초기화 부분은 작성하지 않았습니다.
		 추후에 WeaponDataAsset에 SwingSpeed가 작성되면 아래 주석 부분을 참고하여 SwingSpeed 초기화 부분을 작성하면 됩니다.
		if (WeaponData->SwingSpeed > 0.f)
		{
			SwingSpeed = WeaponData->SwingSpeed;
		}
		*/
	}
}