// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/UseItemActor.h"
#include "Item/UseItemDataAsset.h"


void AUseItemActor::ApplyPickup(ACharacter* Player)
{
	Super::ApplyPickup(Player);

	auto Data = Cast<UUseItemDataAsset>(ItemData);
	if (!Data) return;

	switch (Data->EffectType)
	{
	case EItemType::Heal:
		UE_LOG(LogTemp, Warning, TEXT("즉시 회복 효과 발동: %f"), Data->Value);
		break;

	case EItemType::Damage:
		UE_LOG(LogTemp, Warning, TEXT("즉시 데미지 효과 발동: %f"), Data->Value);
		break;
	}
	
}
