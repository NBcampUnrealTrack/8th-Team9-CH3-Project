#include "Spawn/SpawnManager.h"


ASpawnManager::ASpawnManager()
{
	

}


void ASpawnManager::BeginPlay()
{
	Super::BeginPlay();
	SpawnAtRandomPoint();
}

void ASpawnManager::SpawnAtRandomPoint()
{
	if (EscapeSpawnPoints.IsEmpty() || !EscapingPointActor) return;
	
	int32 RandomIndex = FMath::RandRange(0, EscapeSpawnPoints.Num() - 1);
	AActor* SelectedPoint = EscapeSpawnPoints[RandomIndex];
	
	if (!SelectedPoint) return;
	
	GetWorld()->SpawnActor<AActor>(
		EscapingPointActor,
		SelectedPoint->GetActorLocation(),
		FRotator::ZeroRotator);
}
