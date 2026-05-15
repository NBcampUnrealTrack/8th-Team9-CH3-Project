// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableInterface.h"
#include "GameFramework/Actor.h"
#include "BaseItemActor.generated.h"

class USphereComponent;
class UItemDataAsset;
class UWidgetComponent;

UCLASS()
class IAMLEGEND_API ABaseItemActor : public AActor, public IInteractableInterface
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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> InteractionWidget;

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
	
	UFUNCTION()
	virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// 아이템 적용 (자식에서 override)
	UFUNCTION()
	virtual void ApplyPickup(ACharacter* Player);

	//상호작용 인터페이스 부분
	virtual void Interact(ACharacter* Player) override;
	virtual FString InteractionText() const override;
	virtual void OnPlayerEntered(ACharacter* Player) override;
	virtual void OnPlayerExited(ACharacter* Player) override;
	void InitFromData();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Settings")
	int32 ItemAmount = 1;
};
