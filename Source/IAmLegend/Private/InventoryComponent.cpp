// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "GameFramework/Character.h"
#include "UseItemDataAsset.h"
#include "WeaponDataAsset.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

bool UInventoryComponent::AddItem(UItemDataAsset* NewItem)
{
	if (!NewItem) return false;

	// 이미 인벤토리에 같은 아이템이 있는지 찾기
	for (FItemSlot& Slot : Inventory)
	{
		if (Slot.ItemData == NewItem)
		{
			Slot.Quantity++;
			UE_LOG(LogTemp, Log, TEXT("%s 개수 증가! 현재: %d"), *NewItem->ItemName, Slot.Quantity);
			return true;
		}
	}

	// 없다면 새로 추가
	Inventory.Add(FItemSlot(NewItem, 1));
	return true;
}

void UInventoryComponent::UseItem(int32 Index)
{
	if (!Inventory.IsValidIndex(Index)) return;

	FItemSlot& Slot = Inventory[Index];
    
	// (여기서 아이템 효과 적용 로직 실행...)
	// 예: SelectedItem->UseItem(Player);

	// 사용 후 개수 차감
	Slot.Quantity--;

	if (Slot.Quantity <= 0)
	{
		Inventory.RemoveAt(Index); // 다 쓰면 슬롯 삭제
	}
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter = Cast<ACharacter>(GetOwner());
	
}



