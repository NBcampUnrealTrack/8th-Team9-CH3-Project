#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnManager.generated.h"

UCLASS()
class IAMLEGEND_API ASpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	
	ASpawnManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	//----탈출 지점 설정----
	//랜덤한 탈출 지점 좌표들
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn|Properties")
	TArray<AActor*> EscapeSpawnPoints;
	//탈출 지점 액터
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn|Properties")
	TSubclassOf<AActor> EscapingPointActor;
	//
	UFUNCTION()
	void SpawnEscapingPointAtRandomPoint();

	
	//----적 스폰----
	//스테이지에 적 스폰
	UFUNCTION()
	void SpawnEnemyAtStage();
};
