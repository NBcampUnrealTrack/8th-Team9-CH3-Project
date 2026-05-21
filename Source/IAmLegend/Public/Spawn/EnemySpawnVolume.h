#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawnVolume.generated.h"

class UBoxComponent;
class UDataTable;

UCLASS()
class IAMLEGEND_API AEnemySpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	AEnemySpawnVolume();
	
	//적 스폰
	void TrySpawn(bool IsTimeUp);
	
protected:
	//스폰 공간 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Spawn|Components")
	TObjectPtr<UBoxComponent> SpawnArea;
	//최대 적 스폰 카운트(볼륨 별로 수정 가능)
	UPROPERTY(EditAnywhere, Category="Spawn|Config")
	int32 MaxSpawnCount;
	//스폰할 적 액터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawn|Data")
	TSubclassOf<AActor> ActualEnemyClass;
	
	//적 스폰 시 겹친 체크용 캡슐 반경 (몬스터 크기에 맞게 조절 필요)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawn|SpawnCheck")
	float SpawnCheckCapsuleRadius;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawn|SpawnCheck")
	float SpawnCheckCapsuleHalfHeight;
	//위치 탐색 최대 횟수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawn|SpawnCheck")
	int32 MaxSpawnPlaceCheck;
	
private:
	//스폰할 랜덤할 지점 가져오기
	FVector GetRandomSpawnLocation() const;
	//적 스폰
	void SpawnEnemy(TSubclassOf<AActor> EnemyClass);
	//스폰 위치 탐색
	bool TryGetValidSpawnLocation(FVector& OutLocation);
	
};
