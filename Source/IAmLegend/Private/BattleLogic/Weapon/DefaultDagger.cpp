// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleLogic/Weapon/DefaultDagger.h"
#include "WeaponDataAsset.h"
#include "Kismet/GameplayStatics.h"
#include "Character/HanPlayerCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BattleLogic/TrajectoryComponent.h"

#define ATTACK_TRACE_CHANNEL ECC_GameTraceChannel1

ADefaultDagger::ADefaultDagger()
{
	// 초기값 설정
	WeaponType = EWeaponType::Dagger; // 무기 타입 설정
	StabDamage = 20.f; // 찌르기 공격의 추가 데미지
	StabCooldown = 1.0f; // 찌르기 공격의 쿨다운 시간
	StabDuration = 0.3f; // 찌르기 공격의 지속 시간
	StabRange = 100.f; // 찌르기 공격의 최대 사거리
	StabBoxExtent = FVector(1.f, 40.f, 90.f); // 찌르기 공격의 범위 (박스 형태)
	bIsStabbing = false; // 찌르기 공격 중인지 여부

	// 단검은 투척이 불가능하므로 궤적 컴포넌트를 비활성화
	if (TrajectoryComp)
	{
		TrajectoryComp->PrimaryComponentTick.bCanEverTick = false;
		TrajectoryComp->PrimaryComponentTick.bStartWithTickEnabled = false;
		TrajectoryComp->SetComponentTickEnabled(false);
	}
}

void ADefaultDagger::BeginPlay()
{
	Super::BeginPlay();
}

void ADefaultDagger::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void ADefaultDagger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Mesh || !OwnerCharacter) return;

	if (bIsStabbing)
	{
		StabTrace(); // 찌르기 공격 판정 수행
	}
}

void ADefaultDagger::StartWeaponAttack()
{
	bIsPressingAttack = true; // 공격 버튼이 눌려있는 상태로 설정

	// 이미 공격/찌르기 중이거나 쿨다운 중이거나 소유자 캐릭터가 없는 경우라면 공격 방지
	if (bIsAttacking || bIsStabbing || bIsCooldown || !OwnerCharacter)
	{
		return;
	}

	// 조준 중이 아니라면 근접 무기의 공격 실행
	if (!(OwnerCharacter->IsAiming()))
	{
		ExcuteAttack(); // 실제 공격 처리 (현재는 타이머로 호출하지만, 추후에 공격 애니메이션이 적용되면 애니메이션 노티파이로 대체할 수 있습니다.)
	}

	else
	{
		ExcuteStab(); // 조준 상태에서는 찌르기 공격 실행
	}
}

void ADefaultDagger::ExcuteStab()
{
	// 현재는 타이머 기반으로 동작하지만, 추후에 공격 애니메이션이 적용되면 애니메이션 노티파이로 대체할 수 있습니다.
	// 찌르기가 시작될 때 타이머를 초기화
	GetWorldTimerManager().ClearTimer(AttackTimerHandle);
	GetWorldTimerManager().ClearTimer(AttackIntervalTimerHandle);

	bIsStabbing = true;		// 찌르기 공격 중 상태로 설정
	SetActorTickEnabled(true);	// 틱을 활성화해 타격 판정 시작

	// 공격 판정이 끝나는 시점을 관리하기 위해 타이머 설정
	GetWorldTimerManager().SetTimer(
		AttackTimerHandle,
		this,
		&ADefaultDagger::EndStab,
		StabDuration,
		false
	);
}

void ADefaultDagger::EndStab()
{
	bIsStabbing = false;		// 찌르기 공격 종료 상태로 설정
	bIsCooldown = true;			// 쿨다운 상태로 설정
	SetActorTickEnabled(false);	// 틱을 비활성화해 타격 판정 중지
	HitActors.Empty();			// 타격한 액터 목록 초기화	

	// 쿨타임이 끝나는 시점을 관리하기 위해 타이머 설정
	GetWorldTimerManager().SetTimer(
		AttackIntervalTimerHandle,
		this,
		&AMeleeWeaponBase::FinishCooldown,
		StabCooldown,
		false
	);

	UE_LOG(LogTemp, Warning, TEXT("Stab Attack Ended!"));
}

// 현재 찌르기의 공격 판정은 범위를 기준으로 작동합니다.
// 주석 처리된 메쉬 기준 벡터를 사용하면 메쉬의 특정 소켓 위치를 기준으로 찌르기 공격 판정을 수행할 수 있습니다.
void ADefaultDagger::StabTrace()
{
	if (!Mesh || !OwnerCharacter) return;

	// 찌르기 공격 판정 수행 (현재는 Tick에서 처리하지만, 추후에 공격 애니메이션이 적용되면 애니메이션 노티파이로 대체할 수 있습니다.)
	// 범위 공격 벡터
	FVector ForwardVector = OwnerCharacter->GetActorForwardVector();
	FVector Start = OwnerCharacter->GetActorLocation() + ForwardVector; // 플레이어 앞쪽으로 시작 위치 설정
	FVector End = Start + ForwardVector * StabRange; // 찌르기 공격의 최대 사거리까지 끝 위치 설정

	// 메쉬 기준 벡터
	//FVector Start = Mesh->GetSocketLocation(FName("Root"));
	//FVector End = Mesh->GetSocketLocation(FName("Tip"));

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
		StabBoxExtent,
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

	ProcessHitResults(HitResults); // 타격 결과 처리 (데미지 적용 등)
}

void ADefaultDagger::WeaponInitFromData()
{
	Super::WeaponInitFromData();

	if (UWeaponDataAsset* WeaponData = Cast<UWeaponDataAsset>(ItemData))
	{
		/* 
		if (WeaponData->StabDamage  > 0.f)
		{
			StabDamage  = WeaponData->StabDamage ;
		}
		*/
	}
}