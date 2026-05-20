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
#include "NiagaraSystem.h"
#include "BattleLogic/Weapon/DataAssets/ThrowableWeaponDataAsset.h"

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

}


// Called every frame
void UTrajectoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	DrawTrajectory();
}

void UTrajectoryComponent::InitializeTrajectory(UItemDataAsset* ItemData)
{
	OwnerWeapon = Cast<AThrowableWeaponBase>(GetOwner());

	if (!OwnerWeapon || !ItemData) return;

	if (OwnerWeapon)
	{
		OwnerCharacter = Cast<AHanPlayerCharacter>(OwnerWeapon->GetOwner());
	}

	if (UThrowableWeaponDataAsset* ThrowableData = Cast<UThrowableWeaponDataAsset>(ItemData))
	{
		InitialSpeed = ThrowableData->InitialSpeed;
		GravityScale = ThrowableData->GravityScale;
		MaxSpeed = ThrowableData->MaxSpeed;

		if (!ThrowableData->TrajectorySystemAsset.IsNull())
		{
			if (UNiagaraSystem* LoadedFX = ThrowableData->TrajectorySystemAsset.LoadSynchronous())
			{
				if (NiagaraVisualizer)
				{
					NiagaraVisualizer->SetAsset(LoadedFX);
					NiagaraVisualizer->AttachToComponent(OwnerWeapon->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
				}
			}
		}

		if (!ThrowableData->WeaponSkeletalMesh.IsNull())
		{
			if (USkeletalMesh* LoadedMesh = ThrowableData->WeaponSkeletalMesh.LoadSynchronous())
			{
				FBoxSphereBounds MeshBounds = LoadedMesh->GetBounds();
				FVector MeshSize = MeshBounds.BoxExtent;

				float AutoRadius = FMath::Max(MeshSize.X, MeshSize.Y);
				AutoRadius = FMath::Clamp(AutoRadius, 5.f, 200.f);

				CollisionRadius = AutoRadius;
			}
		}

		bIsTrajectoryEnabled = true;
		EnableTrajectory(false); // 초기에는 궤적 시각화 비활성화
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
	Params.StartLocation = OwnerCharacter->GetMesh()->GetSocketLocation(FName("WeaponSocket"));
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