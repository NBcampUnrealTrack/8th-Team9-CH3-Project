#include "Item/InventoryComponent.h"
#include "GameFramework/Character.h"
#include "Character/HanPlayerCharacter.h"
#include "Item/UseItemDataAsset.h"
#include "Blueprint/UserWidget.h"
#include "Item/InventoryWidget.h"
#include "Item/InventorySlotWidget.h"
#include "Gamemode/MainGameStateBase.h"
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
			FInputModeUIOnly InputMode;
			
			InputMode.SetWidgetToFocus(CraftingWidget->TakeWidget()); 
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            
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

bool UInventoryComponent::AddItem(UItemDataAsset* NewItem, int32 Amount)
{
	// 아이템이 없거나 추가하려는 개수가 0 이하면 바로 리턴
	if (!NewItem || Amount <= 0) return false;

	//전체 인벤토리 (GameInstance 전역 인벤토리) 업데이트
	TArray<FItemSlot>& Inv = GetActualInventory();
	bool bFoundInTotal = false;

	for (FItemSlot& Slot : Inv)
	{
		if (Slot.ItemData == NewItem)
		{
			// 기존에 있던 아이템이면 Amount만큼 더해줌
			Slot.Quantity += Amount; 
			UE_LOG(LogTemp, Log, TEXT("%s 전체 개수 %d 증가 (현재 총합: %d)"), 
				*NewItem->ItemName, Amount, Slot.Quantity);
			bFoundInTotal = true;
			break;
		}
	}

	if (!bFoundInTotal)
	{
		// 새로 추가하는 아이템이면 처음부터 Amount 개수만큼 생성
		Inv.Add(FItemSlot(NewItem, Amount));
	}

	//Gamestate 장부에 추가
	if (GetWorld())
	{
		AMainGameStateBase* GameState = GetWorld()->GetGameState<AMainGameStateBase>();
		if (GameState)
		{
			// GameState에 새로 만든 장부 기록 함수를 호출합니다.
			GameState->AddStageAcquiredItem(NewItem, Amount);
          
			UE_LOG(LogTemp, Log, TEXT("%s 정보를 GameState 장부에 등록 완료!"), *NewItem->ItemName);
		}
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

	UE_LOG(LogTemp, Warning,
		TEXT("=== Current Inventory Status ==="));

	if (Inv.Num() == 0)
	{
		UE_LOG(LogTemp, Log,
			TEXT("Inventory is Empty."));

		ShowInventoryOnScreen();
	}
	else
	{
		TMap<EItemCategory, int32> CategoryCounts;

		for (const FItemSlot& Slot : Inv)
		{
			if (Slot.ItemData)
			{
				UE_LOG(LogTemp, Log,
					TEXT("Item: [%s] | Quantity: %d | Type: %d"),
					*Slot.ItemData->ItemName,
					Slot.Quantity,
					(int32)Slot.ItemData->Category);

				CategoryCounts.FindOrAdd(
					Slot.ItemData->Category) += Slot.Quantity;
			}
		}

		UE_LOG(LogTemp, Warning,
			TEXT("--- Summary by Category ---"));

		for (auto& It : CategoryCounts)
		{
			FString CategoryName =
				StaticEnum<EItemCategory>()
				->GetNameStringByValue((int64)It.Key);

			UE_LOG(LogTemp, Warning,
				TEXT("%s: %d items"),
				*CategoryName,
				It.Value);
		}

		ShowInventoryOnScreen();
	}

	
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

	// 위젯 없으면 생성
	if (!InventoryWidget)
	{
		if (InventoryWidgetClass)
		{
			InventoryWidget =
				CreateWidget<UInventoryWidget>(PC, InventoryWidgetClass);

			if (!InventoryWidget)
			{
				return;
			}
		}
		else
		{
			UE_LOG(LogTemp, Error,
				TEXT("InventoryWidgetClass가 할당되지 않았습니다!"));
			return;
		}
	}

	// 현재 보이는 상태면 숨김
	if (bInventoryVisible)
	{
		InventoryWidget->RemoveFromParent();

		bInventoryVisible = false;

		PC->bShowMouseCursor = false;

		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
	}
	else
	{
		InventoryWidget->AddToViewport();

		InventoryWidget->RefreshInventory(Inv);

		bInventoryVisible = true;

		PC->bShowMouseCursor = true;

		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(
			EMouseLockMode::DoNotLock);

		PC->SetInputMode(InputMode);
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
