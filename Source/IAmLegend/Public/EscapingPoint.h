
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EscapingPoint.generated.h"

class UBoxComponent;
UCLASS()
class IAMLEGEND_API AEscapingPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AEscapingPoint();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Escape|Component")
	USceneComponent* Scene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Escape|Component")
	UBoxComponent* Collision;
	
	//플레이어 오버랩 확인
	UFUNCTION()
	void OnCollisionOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

protected:
	
	virtual void BeginPlay() override;

public:	

};
