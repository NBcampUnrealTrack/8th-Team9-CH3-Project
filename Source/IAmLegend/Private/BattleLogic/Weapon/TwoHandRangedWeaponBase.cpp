// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleLogic/Weapon/TwoHandRangedWeaponBase.h"
#include "WeaponDataAsset.h"
#include "Kismet/GameplayStatics.h"
#include "Character/HanPlayerCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

#define ATTACK_TRACE_CHANNEL ECC_GameTraceChannel1

ATwoHandRangedWeaponBase::ATwoHandRangedWeaponBase()
{
	// 초기값 설정 (추후에 WeaponDataAsset에서 초기화 하는 것으로 변경 예정입니다.)
	WeaponType = EWeaponType::TwoHandedRanged; // 무기 타입 설정
	LeftHandIKSocketName = "LeftHandIKSocket"; // 왼손 IK 소켓 이름 설정
	MeleeAttackRange = 10.f;	// 근접 공격 사거리
	PushForce = 500.f;		// 밀어내는 힘 설정
}

void ATwoHandRangedWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	WeaponInitFromData();
}

void ATwoHandRangedWeaponBase::MeleeAttackTrace()
{
	if (!OwnerCharacter || !Mesh) return;

	// 찌르기 공격 판정 수행 (현재는 Tick에서 처리하지만, 추후에 공격 애니메이션이 적용되면 애니메이션 노티파이로 대체할 수 있습니다.)
	// 범위 공격 벡터
	FVector ForwardVector = OwnerCharacter->GetActorForwardVector();
	float ForwardOffset = FVector::DotProduct(ForwardVector, OwnerCharacter->GetActorLocation() - Mesh->GetComponentLocation());
	FVector Start = OwnerCharacter->GetActorLocation() + ForwardVector * ForwardOffset; // 무기의 위치에서 플레이어의 앞쪽으로 시작 위치 설정
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

void ATwoHandRangedWeaponBase::ProcessMeleeHits(const TArray<FHitResult>& HitResults)
{
	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();

		if (HitActor && !HitActors.Contains(HitActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitActor->GetName());

			// 몬스터 여부를 캐스팅 해야합니다.
			if(ACharacter* HitCharacter = Cast<ACharacter>(HitActor))
			{
				// 해당 부분에서 몬스터의 밀기 함수를 호출합니다.
				// 현재는 단순히 캐릭터에 LaunchCharacter 함수를 사용하였습니다.
				HitCharacter->LaunchCharacter(OwnerCharacter->GetActorForwardVector() * PushForce, true, true); // 예시로 액터를 플레이어의 앞쪽으로 밀어냅니다.
			}
			
			HitActors.Add(HitActor); // 이미 타격한 액터를 추가하여 중복 타격 방지
		}
	}
}