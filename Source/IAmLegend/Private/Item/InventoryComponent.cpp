#include "Item/InventoryComponent.h"
#include "GameFramework/Character.h"
#include "Character/HanPlayerCharacter.h"
#include "Item/UseItemDataAsset.h"
#include "WeaponDataAsset.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
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

void UInventoryComponent::ShowInventory()
{
	UE_LOG(LogTemp, Warning, TEXT("=== Current Inventory Status ==="));

	// 카테고리별 수량을 저장할 맵
	TMap<EItemCategory, int32> CategoryCounts;

	if (Inventory.Num() == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("Inventory is Empty."));
		return;
	}

	for (const FItemSlot& Slot : Inventory)
	{
		if (Slot.ItemData)
		{
			// 상세 목록 출력
			UE_LOG(LogTemp, Log, TEXT("Item: [%s] | Quantity: %d | Type: %d"), 
				*Slot.ItemData->ItemName, 
				Slot.Quantity, 
				(int32)Slot.ItemData->Category);

			// 카테고리별 합계 계산
			CategoryCounts.FindOrAdd(Slot.ItemData->Category) += Slot.Quantity;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("--- Summary by Category ---"));
    
	// 카테고리별 최종 합계 출력
	for (auto& It : CategoryCounts)
	{
		// Enum 값을 문자열로 변환하여 출력하면 더 보기 좋습니다.
		FString CategoryName = StaticEnum<EItemCategory>()->GetNameStringByValue((int64)It.Key);
		UE_LOG(LogTemp, Warning, TEXT("%s: %d items"), *CategoryName, It.Value);
	}
}



