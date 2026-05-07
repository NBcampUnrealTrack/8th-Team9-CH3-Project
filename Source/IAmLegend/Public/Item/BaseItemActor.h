// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseItemActor.generated.h"

class USphereComponent;
class UItemDataAsset;

UCLASS()
class IAMLEGEND_API ABaseItemActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseItemActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Collider;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item")
	TObjectPtr<UItemDataAsset> ItemData;

	// 충돌 이벤트
	UFUNCTION()
	virtual void OnOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	// 아이템 적용 (자식에서 override)
	UFUNCTION()
	virtual void ApplyPickup(ACharacter* Player);

	
	void InitFromData();
};
