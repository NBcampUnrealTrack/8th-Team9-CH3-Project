// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TrajectoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class IAMLEGEND_API UTrajectoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTrajectoryComponent();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Trajectory")
	class UNiagaraComponent* NiagaraVisualizer;

	UPROPERTY(EditAnywhere, Category = "Trajectory")
	TSoftObjectPtr<class UNiagaraSystem> TrajectorySystemAsset;

	class AThrowableWeaponBase* OwnerWeapon; // 투척 무기 참조

	class AHanPlayerCharacter* OwnerCharacter; // 투척 무기를 소유한 캐릭터 참조

	float CollisionRadius; // 투사체의 충돌 반경
	float InitialSpeed; // 투사체의 초기 속도
	float GravityScale; // 투사체의 중력 영향 정도
	float MaxSpeed;		// 투사체의 최대 속도

	bool bIsTrajectoryEnabled; // 궤적 업데이트 활성화 여부

	virtual void BeginPlay() override;

	void UpdateTrajectory(struct FPredictProjectilePathParams& Params); // 투사체의 궤적 업데이트

	void DrawTrajectory(); // 투사체의 궤적 시각화

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void InitializeTrajectory(class UItemDataAsset* ThrowableData); // 투사체의 초기 궤적 설정

	void EnableTrajectory(bool bEnable);

	
};
