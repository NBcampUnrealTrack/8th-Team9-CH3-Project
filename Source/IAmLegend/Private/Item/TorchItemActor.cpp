// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/TorchItemActor.h"
#include "Components/SpotLightComponent.h"

ATorchItemActor::ATorchItemActor()
{
	FlashlightBeam = CreateDefaultSubobject<USpotLightComponent>(TEXT("FlashlightBeam"));
	
	FlashlightBeam->SetupAttachment(Mesh); 
	
	FlashlightBeam->Intensity = 5000.f;
	FlashlightBeam->OuterConeAngle = 30.f;
}

void ATorchItemActor::ToggleTorch(bool bIsOn)
{
	if (FlashlightBeam)
	{
		FlashlightBeam->SetVisibility(bIsOn);
	}
}