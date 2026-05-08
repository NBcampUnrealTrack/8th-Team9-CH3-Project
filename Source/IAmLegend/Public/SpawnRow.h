#pragma once

#include "CoreMinimal.h"
#include "SpawnRow.generated.h"

USTRUCT(BlueprintType)
struct FSpawnRow : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintType)
	FName EnemyName;
	UPROPERTY(EditAnywhere, BlueprintType)
	TSubclassOf<AActor> EnemyClass;
	UPROPERTY(EditAnywhere, BlueprintType)
	float SpawnChance;
};
