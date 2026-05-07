// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleLogic/Weapon/MeleeWeaponBase.h"
#include "WeaponDataAsset.h"
#include "Kismet/GameplayStatics.h"
#include "BattleLogic/Weapon/WeaponProjectileBase.h"
#include "BattleLogic/DummyPlayerCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

#define ATTACK_TRACE_CHANNEL ECC_GameTraceChannel1

void AMeleeWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	SwingSpeed = 1.f; // 기본 휘두르는 속도, 추후에 WeaponDataAsset에서 초기화 하는 것으로 변경 예정입니다.
	AttackDuration = 0.5f; // 기본 공격 지속 시간, 추후에 WeaponDataAsset에서 초기화 하는 것으로 변경 예정입니다.
}

void AMeleeWeaponBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AMeleeWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Mesh) return;

	if (bIsAttacking)
	{
		// 날의 시작과 끝 위치
		FVector Start = Mesh->GetSocketLocation(FName("Root"));
		FVector End = Mesh->GetSocketLocation(FName("Tip"));

		TArray<FHitResult> HitResults;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(GetOwner()); // 플레이어는 충돌에서 제외
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
			{ GetOwner(), this },			// 충돌에서 제외할 액터 배열
			EDrawDebugTrace::ForDuration,   // 디버그 형태 (None, ForOneFrame, ForDuration)
			HitResults,                     // 결과 배열
			true,                           // 자기 자신 무시 여부
			FLinearColor::Red,              // 추적 선 색상
			FLinearColor::Green,            // 히트 시 색상
			1.0f                            // 디버그 선 유지 시간 (초)
		);

		if(bHit && HitResults.Num() > 0)
		{
			for (const FHitResult& Hit : HitResults)
			{
				AActor* HitActor = Hit.GetActor();

				if (HitActor && !HitActors.Contains(HitActor))
				{
					// 타격한 액터에 데미지를 적용
					UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitActor->GetName());
					UGameplayStatics::ApplyDamage(HitActor, Damage, GetOwner()->GetInstigatorController(), GetOwner(), nullptr);
					HitActors.Add(HitActor); // 이미 타격한 액터를 추가하여 중복 타격 방지
				}
			}
		}
	}
}

void AMeleeWeaponBase::WeaponAttack()
{
	if (bIsAttacking) return; // 이미 공격 중이라면 중복 공격 방지

	Super::WeaponAttack();

	// 현재는 타이머 기반으로 동작하지만, 추후에 공격 애니메이션이 적용되면 애니메이션 노티파이로 대체할 수 있습니다.
	// 공격이 시작될 때 타이머를 초기화
	GetWorldTimerManager().ClearTimer(AttackTimerHandle);

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

void AMeleeWeaponBase::EndAttack()
{
	bIsAttacking = false;		// 공격 종료 상태로 설정
	SetActorTickEnabled(false);	// 틱을 비활성화해 타격 판정 중지
	HitActors.Empty();			// 타격한 액터 목록 초기화	

	UE_LOG(LogTemp, Warning, TEXT("Melee Attack Ended!"));
}

// 투척 공격
void AMeleeWeaponBase::SubAttack()
{
	// 투척 공격은 ProjectileClass가 설정되어 있지 않거나 공격 중이면 불가능
	if (!ProjectileClass || !GetOwner() || bIsAttacking) return;

	Super::SubAttack();

	// 칼날의 끝 위치에서 투사체를 생성
	FVector SpawnLocation = Mesh->GetSocketLocation(FName("Tip"));

	// 플레이어의 시점에 따라 투사체의 초기 회전을 설정
	FRotator SpawnRotation;
	if (ADummyPlayerCharacter* OwnerPawn = Cast<ADummyPlayerCharacter>(GetOwner()))
	{
		SpawnRotation = OwnerPawn->GetControlRotation();
	}
	else
	{
		SpawnRotation = GetActorRotation();
	}

	// 투사체 생성
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = GetOwner()->GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AWeaponProjectileBase* SpawnedProjectile = GetWorld()->SpawnActor<AWeaponProjectileBase>(
		ProjectileClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	); 

	Destroy();
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