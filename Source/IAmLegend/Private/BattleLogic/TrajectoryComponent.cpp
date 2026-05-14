// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleLogic/TrajectoryComponent.h"
#include "BattleLogic/Weapon/ThrowableWeaponBase.h"
#include "BattleLogic/Projectile/WeaponProjectileBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/HanPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "NiagaraComponent.h"

#define ENEMY_TRACE_CHANNEL ECC_GameTraceChannel2

// Sets default values for this component's properties
UTrajectoryComponent::UTrajectoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.033f;
	SetComponentTickEnabled(false); // 초기에는 궤적 업데이트 비활성화

	NiagaraVisualizer = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraVisualizer"));
	
	OwnerWeapon = nullptr;

	CollisionRadius = 5.f;
	GravityScale = 1.0f;
	InitialSpeed = 3000.f;
	MaxSpeed = 3000.f;

	bIsTrajectoryEnabled = false;
}


// Called when the game starts
void UTrajectoryComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerWeapon = Cast<AThrowableWeaponBase>(GetOwner());

	if(OwnerWeapon)
	{
		OwnerCharacter = Cast<AHanPlayerCharacter>(OwnerWeapon->GetOwner());
	}

	if (TrajectorySystemAsset && NiagaraVisualizer)
	{
		NiagaraVisualizer->SetAsset(TrajectorySystemAsset);
	}
}


// Called every frame
void UTrajectoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	DrawTrajectory();
}

void UTrajectoryComponent::InitializeTrajectory(TSubclassOf<AWeaponProjectileBase> ProjectileClass)
{
	if (!OwnerCharacter || !OwnerWeapon || !ProjectileClass) return;

	AWeaponProjectileBase* DefaultProjectile = ProjectileClass->GetDefaultObject<AWeaponProjectileBase>();

	if (DefaultProjectile)
	{
		UCapsuleComponent* CollisionComp = DefaultProjectile->FindComponentByClass<UCapsuleComponent>();
		UProjectileMovementComponent* ProjectileMovement = DefaultProjectile->FindComponentByClass<UProjectileMovementComponent>();

		if(CollisionComp)
		{
			CollisionRadius = CollisionComp->GetUnscaledCapsuleRadius();
		}

		if (ProjectileMovement)
		{
			InitialSpeed = ProjectileMovement->InitialSpeed;
			GravityScale = ProjectileMovement->ProjectileGravityScale;
			MaxSpeed = ProjectileMovement->MaxSpeed;
		}

		bIsTrajectoryEnabled = true;
	}
}

void UTrajectoryComponent::EnableTrajectory(bool bEnable)
{
	if (!bIsTrajectoryEnabled || !OwnerCharacter || !OwnerWeapon || !NiagaraVisualizer)
	{
		SetComponentTickEnabled(false);
		return;
	}

	NiagaraVisualizer->ReinitializeSystem();
	NiagaraVisualizer->SetVisibility(bEnable);
	SetComponentTickEnabled(bEnable);
}

void UTrajectoryComponent::UpdateTrajectory(FPredictProjectilePathParams& Params)
{
	Params.StartLocation = OwnerWeapon->Mesh->GetSocketLocation(FName("Tip"));
	Params.LaunchVelocity = OwnerCharacter->GetControlRotation().Vector() * InitialSpeed;
	Params.OverrideGravityZ = GravityScale * GetWorld()->GetGravityZ();

	Params.MaxSimTime = 3.0f;
	Params.SimFrequency = 20.f;

	Params.ProjectileRadius = CollisionRadius;
	Params.bTraceWithCollision = true;
	Params.bTraceWithChannel = false;
	Params.ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	Params.ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	Params.ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ENEMY_TRACE_CHANNEL));

	Params.ActorsToIgnore.Add(OwnerCharacter);
	Params.ActorsToIgnore.Add(OwnerWeapon);
}

void UTrajectoryComponent::DrawTrajectory()
{
	if (!bIsTrajectoryEnabled || !OwnerCharacter || !OwnerWeapon || !NiagaraVisualizer || !OwnerCharacter->IsAiming()) return;

	FPredictProjectilePathParams Params;
	UpdateTrajectory(Params);

	TArray<FVector> PathPoints;
	NiagaraVisualizer->ReinitializeSystem();	// 그리기 전에 지움
	NiagaraVisualizer->SetVariableInt(FName("User.NumPoints"), PathPoints.Num());

	FPredictProjectilePathResult Result;
	UGameplayStatics::PredictProjectilePath(this, Params, Result);

	for (const auto& Data : Result.PathData)
	{
		PathPoints.Add(Data.Location);
	}

	NiagaraVisualizer->SetVariableInt(FName("User.NumPoints"), PathPoints.Num());
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayPosition(
		NiagaraVisualizer,
		FName("User.PathPoints"),
		PathPoints
	);
}