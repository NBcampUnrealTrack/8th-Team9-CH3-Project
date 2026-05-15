// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleLogic/Weapon/PistolBase.h"
#include "WeaponDataAsset.h"
#include "Kismet/GameplayStatics.h"
#include "Character/HanPlayerCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

#define ATTACK_TRACE_CHANNEL ECC_GameTraceChannel1

APistolBase::APistolBase()
{
	// 초기값 설정 (추후에 WeaponDataAsset에서 초기화 하는 것으로 변경 예정입니다.)
	WeaponType = EWeaponType::Pistol; // 무기 타입 설정
	FireRate = 200.f;	// 분당 200발
	MaxAmmo = 12;
	ReloadTime = 2.0f;
	RecoilAmount = 0.2f;
	BaseSpreadAngle = 0.1f;		// 권총은 기본적으로 약간의 탄 퍼짐이 있는 것으로 설정했습니다. 필요에 따라 조정 가능합니다.
	MaxSpreadAngle = 3.f;
	SpreadPerShot = 0.4f;		// 탄 퍼짐과 회복 속도는 두 배 정도가 적당한 것 같습니다. 추후 UI에서 확인 하면서 조정하면 좋을 것 같습니다.
	RecoverySpreadSpeed = 0.8f;	// 무기 별로 다르게 설정가능 합니다

	MeleeDamage = 15.f;	// 근접 공격 데미지
	MeleeAttackRange = 100.f;	// 근접 공격 사거리

	/* 근접 공격용 메시 컴포넌트는 현재 범위를 기준으로 트레이스하기에 필요하지 않아서 주석처리 해두었습니다.
	DaggerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DaggerMesh"));
	DaggerMesh->SetupAttachment(OwnerCharacter->GetMesh(), TEXT("Hand_L_Socket")); // 소켓 이름은 나중에 프로퍼티로 만들 예정

	DaggerMesh->SetHiddenInGame(true);
	DaggerMesh->SetComponentTickEnabled(false);
	DaggerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	*/
}

void APistolBase::BeginPlay()
{
	Super::BeginPlay();
	WeaponInitFromData();
}

// 단검의 찌르기 로직과 동일합니다.
void APistolBase::MeleeAttackTrace()
{
	if (!OwnerCharacter) return; // 해당 부분에서 단검 메시 검사를 추가해야 합니다.

	// 범위 공격 벡터
	FVector ForwardVector = OwnerCharacter->GetActorForwardVector();
	FVector Start = OwnerCharacter->GetActorLocation() + ForwardVector; // 플레이어 앞쪽으로 시작 위치 설정
	FVector End = Start + ForwardVector * MeleeAttackRange;

	/* 메시 기준 벡터
	// 메시 기준 피격 판정을 한다면 헤더의 메시 컴포넌트를 활성화하고 아래의 코드를 사용하면 됩니다.
	FVector Start = Mesh->GetSocketLocation(FName("Root"));
	FVector End = Mesh->GetSocketLocation(FName("Tip"));
	*/

	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params;

	Params.AddIgnoredActor(OwnerCharacter); // 플레이어는 충돌에서 제외
	Params.AddIgnoredActor(this);		// 자신은 충돌에서 제외

	/* 박스 형태의 트레이스
	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		ATTACK_TRACE_CHANNEL,
		FCollisionShape::MakeBox(StabBoxExtent),
		Params
	);
	*/

	// 디버그 용
	bool bHit = UKismetSystemLibrary::BoxTraceMulti(
		GetWorld(),
		Start,
		End,
		MeleeAttackBoxExtent,
		OwnerCharacter->GetActorRotation(),
		UEngineTypes::ConvertToTraceType(ATTACK_TRACE_CHANNEL),
		false,
		{ OwnerCharacter, this },
		EDrawDebugTrace::ForDuration,
		HitResults,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		1.0f
	);

	ProcessMeleeHits(HitResults); // 타격 결과 처리 (데미지 적용 등)
}

void APistolBase::ProcessMeleeHits(const TArray<FHitResult>& HitResults)
{
	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && !HitActors.Contains(HitActor))
		{
			// 타격한 액터에 데미지를 적용
			UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitActor->GetName());
			UGameplayStatics::ApplyDamage(HitActor, MeleeDamage, OwnerCharacter->GetInstigatorController(), OwnerCharacter, nullptr);
			HitActors.Add(HitActor); // 이미 타격한 액터를 추가하여 중복 타격 방지
		}
	}
}