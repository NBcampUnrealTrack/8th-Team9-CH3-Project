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
	case EItemType::Escape:
		UE_LOG(LogTemp, Warning, TEXT("캐릭터가 탈출 아이템을 주웠다!"));
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,1.0f,FColor::Blue, 
				FString::Printf(TEXT("캐릭터가 %s 아이템 획득! "), *Data->ItemName));
		}
		break;

	case EItemType::Heal:
		UE_LOG(LogTemp, Warning, TEXT("캐릭터가 %s 아이템 : 회복 아이템을 획득!"), *Data->ItemName);
		break;
	}
	
}
