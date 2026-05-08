#include "EnemySpawnVolume.h"
#include "Components/BoxComponent.h"


AEnemySpawnVolume::AEnemySpawnVolume()
{
	Scene = CreateDefaultSubobject<USceneComponent>("Scene");
	SetRootComponent(Scene);
	
	SpawnArea = CreateDefaultSubobject<UBoxComponent>("SpawnArea");
	SpawnArea->SetupAttachment(Scene);

	
	MaxSpawnCount = 5;
}

void AEnemySpawnVolume::TrySpawn()
{
	for (int32 i = 0; i<MaxSpawnCount; ++i)
	{
		SpawnEnemy(Enemy);
	}
}

FVector AEnemySpawnVolume::GetRandomSpawnLocation() const
{
	FVector BoxExtent = SpawnArea->GetScaledBoxExtent();
	FVector BoxOrigin = SpawnArea->GetComponentLocation();
	
	FVector RandPoint = FVector(
		FMath::FRandRange(BoxOrigin.X - BoxExtent.X, BoxOrigin.X + BoxExtent.X),
		FMath::FRandRange(BoxOrigin.Y - BoxExtent.Y, BoxOrigin.Y + BoxExtent.Y),
		FMath::FRandRange(BoxOrigin.Z - BoxExtent.Z, BoxOrigin.Z + BoxExtent.Z));	
	
	return RandPoint;
}

void AEnemySpawnVolume::SpawnEnemy(TSubclassOf<AActor> EnemyClass)
{
	if (!EnemyClass) return;
	
	FRotator RandRotation = FRotator(0.0f, FMath::FRandRange(0.0f, 360.0f), 0.0f);
	
	GetWorld()->SpawnActor<AActor>(
		EnemyClass,
		GetRandomSpawnLocation(),
		RandRotation);
}
