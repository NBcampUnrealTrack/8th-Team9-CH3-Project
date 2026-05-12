
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EscapingPoint.generated.h"

class UBoxComponent;
class UParticleSystemComponent;
class UPointLightComponent;
UCLASS()
class IAMLEGEND_API AEscapingPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AEscapingPoint();
	
	//컴포넌트 및 파티클
	UPROPERTY(VisibleAnywhere, Category="Escape|Component")
	TObjectPtr<UBoxComponent> Collision;
	UPROPERTY(VisibleAnywhere, Category="Escape|Component")
	TObjectPtr<UStaticMeshComponent> StaticMesh;
	UPROPERTY(VisibleAnywhere, Category = "Escape|Component")
	TObjectPtr<UPointLightComponent> GreenLight;
	UPROPERTY(VisibleAnywhere, Category = "Escape|Component")
	TObjectPtr<UParticleSystemComponent> SmokeParticle;
	
	//탈출 대기 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Escape|Time")
	float EscapeDuration;
	
	//플레이어 오버랩 및 오버랩 종료 확인
	UFUNCTION()
	void OnCollisionOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
	UFUNCTION()
	void OnCollisionEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);
	
private:	
	//탈출 타이머
	FTimerHandle EscapeTimer;
	//로그 타이머
	FTimerHandle LogTimer;
	void RunLogTimer();
	
	
	
	//플레이어 탈출 성공 시 호출
	void PlayerEscaped();
};
