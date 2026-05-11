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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn|Properties")
	TArray<AActor*> EscapeSpawnPoints;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn|Properties")
	TSubclassOf<AActor> EscapingPointActor;
	
	UFUNCTION()
	void SpawnAtRandomPoint();

};
