// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class IAMLEGEND_API UItemDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMesh> ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class ABaseItemActor> ItemActorClass;
	
};
