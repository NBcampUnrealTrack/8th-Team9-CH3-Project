#include "Spawn/SpawnManager.h"
#include "Spawn/EnemySpawnVolume.h"
#include "Kismet/GameplayStatics.h"
#include "Gamemode/MainGameModeBase.h"

void ASpawnManager::BeginPlay()
{
	Super::BeginPlay();
	SpawnEscapingPointAtRandomPoint();
}

//탈출 지점 지정한 포인트 중 랜덤한 한곳에 스폰
void ASpawnManager::SpawnEscapingPointAtRandomPoint()
{
	//지정된 탈출 포인트 확인 && 탈출 지점 액터 확인
	if (EscapeSpawnPoints.IsEmpty() || !EscapingPointActor) return;
	
	//설정한 지정된 탈출 포인트 중 하나 랜덤으로 가져오기
	int32 RandomIndex = FMath::RandRange(0, EscapeSpawnPoints.Num() - 1);
	AActor* SelectedPoint = EscapeSpawnPoints[RandomIndex];
	
	if (!SelectedPoint) return;
	
	//탈출 지점 스폰
	GetWorld()->SpawnActor<AActor>(
		EscapingPointActor,
		SelectedPoint->GetActorLocation(),
		FRotator::ZeroRotator);
}

//스테이지에 적 스폰
void ASpawnManager::SpawnEnemyAtStage(bool bIsStageTimeUp)
{
	//스테이지에 있는 모든 스폰 볼륨 가져오기
	TArray<AActor*> EnemySpawnVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawnVolume::StaticClass(), EnemySpawnVolumes);
	if (EnemySpawnVolumes.Num() <= 0) return;
	
	
	//모든 스폰 볼륨에 적 스폰
	for (int32 i=0; i<EnemySpawnVolumes.Num(); ++i)
	{
		AEnemySpawnVolume* EnemySpawnVolume = Cast<AEnemySpawnVolume>(EnemySpawnVolumes[i]);
		EnemySpawnVolume->TrySpawn(bIsStageTimeUp);
	}
}
