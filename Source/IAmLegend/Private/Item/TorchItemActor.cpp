// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/TorchItemActor.h"
#include "Components/SpotLightComponent.h"
#include "Components/SphereComponent.h" 
#include "Components/WidgetComponent.h"

void ATorchItemActor::ToggleTorch(bool bIsOn)
{
	if (FlashlightBeam)
	{
		FlashlightBeam->SetVisibility(bIsOn);
	}
}
ATorchItemActor::ATorchItemActor()
{
	FlashlightBeam = CreateDefaultSubobject<USpotLightComponent>(TEXT("FlashlightBeam"));
	
	FlashlightBeam->SetupAttachment(Mesh, TEXT("LightSocket"));
	
	FlashlightBeam->Intensity = 5000.f;
	FlashlightBeam->OuterConeAngle = 30.f;
}



void ATorchItemActor::BeginPlay()
{
	
	Super::BeginPlay();
	
	if (Collider)
	{
		
		Collider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Collider->SetGenerateOverlapEvents(false);
	}

	if (InteractionWidget)
	{
		
		InteractionWidget->SetVisibility(false);
		InteractionWidget->SetActive(false);
	}
	// -------------------------------------------------------------------------

	
	ToggleTorch(true);
}