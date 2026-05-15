#include "Item/InventoryComponent.h"
#include "GameFramework/Character.h"
#include "Character/HanPlayerCharacter.h"
#include "Item/UseItemDataAsset.h"
#include "Blueprint/UserWidget.h"
#include "Item/InventoryWidget.h"
#include "Item/InventorySlotWidget.h"
#include "Gamemode/MainGameInstance.h"
#include "WeaponDataAsset.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UInventoryComponent::ToggleCraftingUI(bool bShow)
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	if (bShow)
	{
		if (!CraftingWidget && CraftingWidgetClass)
		{
			CraftingWidget = CreateWidget<UUserWidget>(PC, CraftingWidgetClass);
		}

		if (CraftingWidget)
		{
			CraftingWidget->AddToViewport();
            
			// 마우스 커서 활성화 및 입력 모드 변경
			PC->bShowMouseCursor = true;
			FInputModeGameAndUI InputMode;
			PC->SetInputMode(InputMode);
		}
	}
	else
	{
		if (CraftingWidget)
		{
			CraftingWidget->RemoveFromParent();
            
			// 다시 게임으로 돌아가기
			PC->bShowMouseCursor = false;
			FInputModeGameOnly InputMode;
			PC->SetInputMode(InputMode);
		}
	}
}

TArray<FItemSlot>& UInventoryComponent::GetActualInventory()
{
	UMainGameInstance* GI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance());
	if (GI)
	{
		return GI->GlobalInventory;
	}
	
	static TArray<FItemSlot> EmptyInv;
	return EmptyInv;
}

bool UInventoryComponent::AddItem(UItemDataAsset* NewItem)
{
	if (!NewItem) return false;

	//  전체 인벤토리 업데이트
	TArray<FItemSlot>& Inv = GetActualInventory();
	bool bFoundInTotal = false;
	for (FItemSlot& Slot : Inv)
	{
		if (Slot.ItemData == NewItem)
		{
			Slot.Quantity++;
			UE_LOG(LogTemp, Log, TEXT("%s 전체 개수 증가: %d"), *NewItem->ItemName, Slot.Quantity);
			bFoundInTotal = true;
			break;
		}
	}

	if (!bFoundInTotal)
	{
		Inv.Add(FItemSlot(NewItem, 1));
	}

	// 이번 스테이지 획득 아이템용
	// Shelter 맵이 아닐 때만 기록하고 싶다면 여기에 if문을 추가할 수 있음
	// 추후에 필요시 추가하겠습니다
	bool bFoundInStage = false;
	for (FItemSlot& Slot : CurrentStageAcquiredItems)
	{
		if (Slot.ItemData == NewItem)
		{
			Slot.Quantity++;
			bFoundInStage = true;
			break;
		}
	}

	if (!bFoundInStage)
	{
		CurrentStageAcquiredItems.Add(FItemSlot(NewItem, 1));
	}

	return true; 
}
void UInventoryComponent::UseItem(int32 Index)
{
	TArray<FItemSlot>& Inv = GetActualInventory();
	if (!Inv.IsValidIndex(Index)) return;

	FItemSlot& Slot = Inv[Index];
    
	

	// 사용 후 개수 차감
	Slot.Quantity--;

	if (Slot.Quantity <= 0)
	{
		Inv.RemoveAt(Index); // 다 쓰면 슬롯 삭제
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
	TArray<FItemSlot>& Inv = GetActualInventory();
	UE_LOG(LogTemp , Warning, TEXT("=== Current Inventory Status ==="));

	
	if (Inv.Num() == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("Inventory is Empty."));
		ShowInventoryOnScreen(); // 여기서 한 번만 호출하고 끝냄
		return; 
	}

	
	TMap<EItemCategory, int32> CategoryCounts;
	for (const FItemSlot& Slot : Inv)
	{
		if (Slot.ItemData)
		{
			UE_LOG(LogTemp, Log, TEXT("Item: [%s] | Quantity: %d | Type: %d"), 
			   *Slot.ItemData->ItemName, Slot.Quantity, (int32)Slot.ItemData->Category);

			CategoryCounts.FindOrAdd(Slot.ItemData->Category) += Slot.Quantity;
		}
	}


	UE_LOG(LogTemp, Warning, TEXT("--- Summary by Category ---"));
	for (auto& It : CategoryCounts)
	{
		FString CategoryName = StaticEnum<EItemCategory>()->GetNameStringByValue((int64)It.Key);
		UE_LOG(LogTemp, Warning, TEXT("%s: %d items"), *CategoryName, It.Value);
	}
    
	
	ShowInventoryOnScreen();
	DisplayUI();
}

void UInventoryComponent::ShowInventoryOnScreen()
{
	TArray<FItemSlot>& Inv = GetActualInventory();
	if (!GEngine) return;

	if (Inv.Num() == 0)
	{
		GEngine->AddOnScreenDebugMessage(100, 3.0f, FColor::Red, TEXT("Inventory is Empty."));
		return;
	}

	
	FString FullInventoryText = TEXT("=== Current Inventory ===\n");

	for (const FItemSlot& Slot : Inv)
	{
		if (Slot.ItemData)
		{
			FullInventoryText += FString::Printf(TEXT("Item: [%s] | Quantity: %d\n"), 
				*Slot.ItemData->ItemName, 
				Slot.Quantity);
		}
	}

	
	GEngine->AddOnScreenDebugMessage(100, 5.0f, FColor::Cyan, FullInventoryText);
}

void UInventoryComponent::DisplayUI()
{
	TArray<FItemSlot>& Inv = GetActualInventory();

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	// 1. 위젯이 없는 경우 생성 시도
	if (!InventoryWidget)
	{
		// InventoryWidgetClass가 에디터에서 선택되었는지 확인 필수!
		if (InventoryWidgetClass) 
		{
			InventoryWidget = CreateWidget<UInventoryWidget>(PC, InventoryWidgetClass);
            
			if (InventoryWidget)
			{
				InventoryWidget->AddToViewport();
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("InventoryWidgetClass가 할당되지 않았습니다!"));
			return; // 클래스가 없으면 아래 코드를 실행하면 안 됨
		}
	}

	// 2. 최종적으로 위젯이 유효한지 확인 후 함수 호출
	if (InventoryWidget)
	{
		InventoryWidget->RefreshInventory(Inv);
	}
}

int32 UInventoryComponent::GetItemQuantity(UItemDataAsset* TargetItem)
{
	TArray<FItemSlot>& Inv = GetActualInventory();
	int32 Total = 0;
	for (const FItemSlot& Slot : Inv)
	{
		if (Slot.ItemData == TargetItem) Total += Slot.Quantity;
	}
	return Total;
}

void UInventoryComponent::RemoveItemQuantity(UItemDataAsset* TargetItem, int32 Amount)
{
	TArray<FItemSlot>& Inv = GetActualInventory();
	for (int32 i = Inv.Num() - 1; i >= 0; i--) // 뒤에서부터 순회하며 삭제
	{
		if (Inv[i].ItemData == TargetItem)
		{
			if (Inv[i].Quantity > Amount)
			{
				Inv[i].Quantity -= Amount;
				return;
			}
			else
			{
				Amount -= Inv[i].Quantity;
				Inv.RemoveAt(i);
			}
		}
		if (Amount <= 0) break;
	}
}
