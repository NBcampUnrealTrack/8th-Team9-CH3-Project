#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawnVolume.generated.h"

class UBoxComponent;

UCLASS()
class IAMLEGEND_API AEnemySpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	AEnemySpawnVolume();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Spawn|Components")
	USceneComponent* Scene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Spawn|Components")
	UBoxComponent* SpawnArea;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawn|Data")
	UDataTable* EnemyDataTable;
	UPROPERTY(EditAnywhere, Category="Spawn|Config")
	int32 MaxSpawnCount;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawn|Data")
	TSubclassOf<AActor> Enemy;
	//적 스폰
	void TrySpawn();
	
private:
	//스폰할 랜덤할 지점 가져오기
	UFUNCTION(BlueprintCallable, Category="Spawn")
	FVector GetRandomSpawnLocation() const;
	//적 스폰
	UFUNCTION(BlueprintCallable, Category="Spawn")
	void SpawnEnemy(TSubclassOf<AActor> EnemyClass);
	
};
