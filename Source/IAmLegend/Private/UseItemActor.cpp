// Fill out your copyright notice in the Description page of Project Settings.


#include "UseItemActor.h"
#include "UseItemDataAsset.h"

void AUseItemActor::ApplyPickup(ACharacter* Player)
{
	auto Data = Cast<UUseItemDataAsset>(ItemData);
	if (!Data) return;

	switch (Data->EffectType)
	{
	case EItemType::Heal:
		UE_LOG(LogTemp, Warning, TEXT("Heal %f"), Data->Value);
		break;

	case EItemType::Damage:
		UE_LOG(LogTemp, Warning, TEXT("Damage %f"), Data->Value);
		break;
	}

	Destroy();
}
