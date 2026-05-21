#include "Spawn/EnemySpawnVolume.h"

#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"


AEnemySpawnVolume::AEnemySpawnVolume()
{
	SpawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnArea"));
	SetRootComponent(SpawnArea);
	
	//최대 스폰 카운트
	MaxSpawnCount = 5;
	
	//겹침 체크용 캡슐 반경
	SpawnCheckCapsuleRadius = 10.f;
	SpawnCheckCapsuleHalfHeight = 80.f;
	//적 스폰 위치 탐색 최대 시도 횟숫
	MaxSpawnPlaceCheck = 10;
}

//적 스폰 시도
void AEnemySpawnVolume::TrySpawn(bool IsTimeUp)
{
	//제한 시간 지났으면 2배 스폰
	if (!IsTimeUp)
	{
		for (int32 i = 0; i<MaxSpawnCount; ++i)
		{
			SpawnEnemy(ActualEnemyClass);
		}
	}
	else
	{
		for (int32 i = 0; i<MaxSpawnCount*2; ++i)
		{
			SpawnEnemy(ActualEnemyClass);
		}
	}
}

//스폰 볼륨 안에서 랜덤한 좌표 가져오기
FVector AEnemySpawnVolume::GetRandomSpawnLocation() const
{
	FVector BoxExtent = SpawnArea->GetScaledBoxExtent();
	FVector BoxOrigin = SpawnArea->GetComponentLocation();
	
	FVector RandPoint = FVector(
		FMath::FRandRange(BoxOrigin.X - BoxExtent.X, BoxOrigin.X + BoxExtent.X),
		FMath::FRandRange(BoxOrigin.Y - BoxExtent.Y, BoxOrigin.Y + BoxExtent.Y),
		BoxOrigin.Z + BoxExtent.Z);	
	
	return RandPoint;
}

//적 스폰
void AEnemySpawnVolume::SpawnEnemy(TSubclassOf<AActor> EnemyClass)
{
	if (!EnemyClass) return;
	
	FRotator RandRotation = FRotator(0.0f, FMath::FRandRange(0.0f, 360.0f), 0.0f);
	
	FVector SpawnLocation;
	
	//볼륨 안에 안겹치고 스폰할 자리 있는지 확인. 공간이 없다면 스폰 스킵
	if (!TryGetValidSpawnLocation(SpawnLocation)) return;
	
	//적 스폰
	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
		EnemyClass,
		SpawnLocation,
		RandRotation);	
}

//바닥 위치 확인, 주변에 겹치는 사물이 있는지 확인 및 스폰 위치 반환
bool AEnemySpawnVolume::TryGetValidSpawnLocation(FVector& OutLocation)
{
	FVector BoxExtent = SpawnArea->GetScaledBoxExtent();
	FVector BoxOrigin = SpawnArea->GetComponentLocation();
	float TraceEndZ = BoxOrigin.Z - BoxExtent.Z - 200.f;;
	
	//스폰 가능한 위치 있는지 확인, MaxSpawnPlaceCheck만큼 확인(10번)
	for (int32 i = 0; i<MaxSpawnPlaceCheck; ++i)
	{
		FVector RandLocation = GetRandomSpawnLocation();
		
		//바닥 찾기
		FHitResult GroundHit;
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(this);
		
		bool bHitGround = UKismetSystemLibrary::LineTraceSingle(
			this,
			RandLocation,
			FVector(RandLocation.X, RandLocation.Y, TraceEndZ),
			UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
			false,
			ActorsToIgnore,
			EDrawDebugTrace::ForDuration,
			GroundHit,
			true);
		
		//바닥이 없으면 스폰X
		if (!bHitGround) continue;
		
		//겹침 체크
		FVector CapsuleCenter  = GroundHit.ImpactPoint + FVector(0.f, 0.f, SpawnCheckCapsuleHalfHeight+1.f);
		
		TArray<AActor*> Overlaps;
		//주변에 겹치는 사물 있는지 캡슐로 확인
		bool bOverLapping = UKismetSystemLibrary::CapsuleOverlapActors(
			this,
			CapsuleCenter,
			SpawnCheckCapsuleRadius,
			SpawnCheckCapsuleHalfHeight,
			TArray<TEnumAsByte<EObjectTypeQuery>>{
				UEngineTypes::ConvertToObjectType(ECC_WorldStatic),
				UEngineTypes::ConvertToObjectType(ECC_Pawn)
			},
			nullptr,
			ActorsToIgnore,
			Overlaps);
		
		//디버그
		if (bOverLapping)
		{
			// 겹침 있으면 빨강
			DrawDebugCapsule(
				GetWorld(),
				CapsuleCenter,
				SpawnCheckCapsuleHalfHeight,
				SpawnCheckCapsuleRadius,
				FQuat::Identity,
				FColor::Red,
				false,
				30.f
			);
		}
		
		if (!bOverLapping)
		{
			// 유효한 위치면 초록
			DrawDebugCapsule(
				GetWorld(),
				CapsuleCenter,
				SpawnCheckCapsuleHalfHeight,
				SpawnCheckCapsuleRadius,
			FQuat::Identity,
				FColor::Green,
				false,
				30.f
			);
			OutLocation = GroundHit.ImpactPoint + FVector(0.f, 0.f, SpawnCheckCapsuleHalfHeight);
			//UE_LOG(LogTemp, Warning, TEXT("ImpactPoint: %s"), *GroundHit.ImpactPoint.ToString());
			return true;
		}
	}
	
	return false;
}
