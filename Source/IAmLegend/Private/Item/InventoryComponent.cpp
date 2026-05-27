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
#include "UI/MainHUD.h"
#include "PlayerHealthWidget.h"
#include "BattleLogic/Attachment/AttachmentDataAsset.h"
#include "BattleLogic/Weapon/RangedWeaponBase.h"
#include "BattleLogic/Attachment/RangedAttachmentComponent.h"

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
		return GI->GetItemsFromGlobalInventory();
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
			
			GameState->AddStageAcquiredItem(NewItem, Amount);
          
			UE_LOG(LogTemp, Log, TEXT("%s 정보를 GameState 장부에 등록 완료!"), *NewItem->ItemName);
		}
	}

	return true;
}

// InventoryComponent.cpp의 UseItem 함수 수정

void UInventoryComponent::UseItem(int32 Index)
{
	TArray<FItemSlot>& Inv = GetActualInventory();
    
	// 1. 유효한 인덱스인지 먼저 방어 검사
	if (!Inv.IsValidIndex(Index)) return;

	FItemSlot& Slot = Inv[Index];
	if (!Slot.ItemData) return;

	// 2. 소유주 캐릭터를 우리가 만든 HanPlayerCharacter로 안전하게 캐스팅
	AHanPlayerCharacter* PlayerChar = Cast<AHanPlayerCharacter>(OwnerCharacter);
	if (!PlayerChar)
	{
		UE_LOG(LogTemp, Error, TEXT("아이템 사용 실패: 캐스팅할 수 없는 캐릭터이거나 유효하지 않습니다."));
		return;
	}

	// ==========================================
	// [코드 추가] 무기 아이템인지 확인 후 장착 - 김민성
	// ==========================================
	if (UWeaponDataAsset* WeaponData = Cast<UWeaponDataAsset>(Slot.ItemData))
	{
		// 무기 장착 함수 호출
		PlayerChar->EquipWeapon(WeaponData);

		// 장착 후 인벤토리 UI 즉시 새로고침
		DisplayUI(true);

		return;
	}

	// ==========================================
	// [코드 추가] 부착물 아이템인지 확인 후 장착 - 차재현
	// ==========================================
	if (UAttachmentDataAsset* AttachmentData = Cast<UAttachmentDataAsset>(Slot.ItemData))
	{
		TMap<EWeaponSlot, AWeaponBase*>& WeaponSlots = PlayerChar->GetWeaponSlots();
		if(!WeaponSlots.Contains(EWeaponSlot::Ranged)) return;

		ARangedWeaponBase* RangedWeapon = Cast<ARangedWeaponBase>(WeaponSlots[EWeaponSlot::Ranged]);
		if(!RangedWeapon) return;
		
		URangedAttachmentComponent* AttachmentComp = RangedWeapon->GetRangedAttachmentComponent();
		if(!AttachmentComp->IsCanEquipAttachment(AttachmentData)) return;

		// 기존 장착된 부착물 제거 및 인벤토리에 추가
		if(UAttachmentDataAsset* OldAttachmentData = AttachmentComp->UnequipAttachment(AttachmentData->AttachmentSlot))
		{
			AddItem(OldAttachmentData, 1);
		}

		// 새 부착물 장착
		if (AttachmentComp->EquipAttachment(AttachmentData))
		{
			Slot.Quantity--;
			if (Slot.Quantity <= 0)
			{
				Inv.RemoveAt(Index);
			}
		}
		
		// 장착 후 인벤토리 UI 즉시 새로고침
		DisplayUI(true);
		return;
	}

	// 3. 사용 가능한 아이템 데이터 에셋(UUseItemDataAsset)인지 검사
	UUseItemDataAsset* UseItemData = Cast<UUseItemDataAsset>(Slot.ItemData);
	if (!UseItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] 제품은 사용(소모)할 수 있는 아이템 형태가 아닙니다."), *Slot.ItemData->ItemName);
		return;
	}

	// ==========================================
	// [조건 분기 시작]
	// ==========================================
    
	// 4-A. 이펙트 타입이 Heal(회복)인 경우
	if (UseItemData->EffectType == EItemType::Heal)
	{
		float CurrentHP = PlayerChar->GetHealth();
		float MaxHP = PlayerChar->GetMaxHealth();

		// [체력 체크] 이미 피가 가득 차 있다면 취소
		/*if (CurrentHP >= MaxHP)
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange, TEXT("체력이 이미 다 차 있습니다!"));
			}
			UE_LOG(LogTemp, Log, TEXT("아이템 사용 취소: 현재 체력이 가득 차 있습니다."));
			return; 
		}*/

		// [회복 적용]
		float NewHP = FMath::Clamp(CurrentHP + UseItemData->Value, 0.f, MaxHP);
		PlayerChar->SetHealth(NewHP);

		// UI 체력바 강제 업데이트
		APlayerController* PC = Cast<APlayerController>(PlayerChar->GetController());
		if (PC)
		{
			AMainHUD* MainHUD = Cast<AMainHUD>(PC->GetHUD());
			if (MainHUD && MainHUD->GetPlayerHealthWidget())
			{
				MainHUD->GetPlayerHealthWidget()->UpdateHP(NewHP, MaxHP);
				UE_LOG(LogTemp, Log, TEXT("인벤토리 컴포넌트: 체력 UI 강제 업데이트 성공!"));
			}
		}

		/*if (GEngine)
		{
			FString TurnText = FString::Printf(TEXT("아이템 사용함: %s | 체력 회복 완료! (+%.1f) 현재: %.1f / %.1f"), 
				*UseItemData->ItemName, UseItemData->Value, NewHP, MaxHP);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TurnText);
		}
		*/

		// 힐 성공 시에만 수량 차감 및 슬롯 제거
		Slot.Quantity--;
		if (Slot.Quantity <= 0)
		{
			Inv.RemoveAt(Index); 
		}

		UE_LOG(LogTemp, Log, TEXT("[%s] 회복 완료. 남은 수량: %d"), *UseItemData->ItemName, Slot.Quantity);
        
		// 인벤토리 UI 갱신
		DisplayUI(true);
	}
	// 4-B. 이펙트 타입이 Buff(버프)이고 이름이 "각성제"인 경우 (Heal 블록 바깥에 위치해야 함!)
	else if (UseItemData->EffectType == EItemType::Buff && UseItemData->ItemName.Equals(TEXT("각성제")))
	{
		// 현재 캐릭터의 기본 이동속도를 가져옴
		float CurrentSpeed = PlayerChar->GetBaseWalkSpeed();
    
		// 아이템의 Value만큼 속도 상승 비율 적용 (예: 0.2 이면 1.2배인 20% 상승)
		float BuffedSpeed = CurrentSpeed * (1.0f + UseItemData->Value);
    
		// 캐릭터에 새로운 속도 즉시 영구 지정
		PlayerChar->SetBaseWalkSpeed(BuffedSpeed);

		/*if (GEngine)
		{
			FString BuffText = FString::Printf(TEXT("아이템 사용함: %s | 이동 속도 %.0f%% 영구 상승! (현재 속도: %.1f)"), 
			 *UseItemData->ItemName, UseItemData->Value * 100.f, BuffedSpeed);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, BuffText);
		}*/
		UE_LOG(LogTemp, Log, TEXT("[%s] 버프 영구 발동. 속도 갱신: %.1f -> %.1f"), *UseItemData->ItemName, CurrentSpeed, BuffedSpeed);

		// 버프 성공 시에만 수량 차감 및 슬롯 제거
		Slot.Quantity--;
		if (Slot.Quantity <= 0)
		{
			Inv.RemoveAt(Index);
		}

		// 인벤토리 UI 갱신
		DisplayUI(true);
	}
	// 4-C. 그 외 정의되지 않은 아이템 타입인 경우
	else if (UseItemData->EffectType == EItemType::Buff && UseItemData->ItemName.Equals(TEXT("진통제")))
	{
		// 1. 플레이어 자체의 배율 스탯도 올려둠 (추후 새로 주울 무기를 위해)
		PlayerChar->MeleeDamageModifier *= UseItemData->Value;

	
		for (const TPair<EWeaponSlot, AWeaponBase*>& Elem : PlayerChar->GetWeaponSlots())
		{
			AWeaponBase* Weapon = Elem.Value;
			if (IsValid(Weapon))
			{
				// 그 무기가 근접(Melee)이거나 단검(Dagger) 슬롯에 있다면 데미지 즉시 상승!
				if (Elem.Key == EWeaponSlot::Melee || Elem.Key == EWeaponSlot::Dagger)
				{
				
					float CurrentDamage = Weapon->GetDamage();
					Weapon->SetDamage(CurrentDamage * UseItemData->Value);
                    
					UE_LOG(LogTemp, Log, TEXT("무기 [%s] 데미지 버프 적용 완료! 현재 데미지: %.1f"), 
					   *Weapon->GetName(), Weapon->GetDamage());
				}
			}
		}
		// --------------------------------------------------------------

		/*if (GEngine)
		{
			FString BuffText = FString::Printf(TEXT("아이템 사용함: %s | 보유한 모든 근접 무기 데미지 %.1f배 영구 상승!"), 
			*UseItemData->ItemName, UseItemData->Value);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, BuffText);
		}*/

		// 수량 차감 및 슬롯 제거
		Slot.Quantity--;
		if (Slot.Quantity <= 0)
		{
			Inv.RemoveAt(Index);
		}

		DisplayUI(true);
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

	
	DisplayUI(true);
}

void UInventoryComponent::ShowInventoryOnScreen()
{
	TArray<FItemSlot>& Inv = GetActualInventory();
	/*if (!GEngine) return;

	if (Inv.Num() == 0)
	{
		GEngine->AddOnScreenDebugMessage(100, 3.0f, FColor::Red, TEXT("Inventory is Empty."));
		return;
	}*/

	
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

	
	/*GEngine->AddOnScreenDebugMessage(100, 5.0f, FColor::Cyan, FullInventoryText);*/
}

void UInventoryComponent::DisplayUI(bool bShow)
{
	TArray<FItemSlot>& Inv = GetActualInventory();
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	if (bShow)
	{
		if (bInventoryVisible)
		{
			if (InventoryWidget)
			{
				InventoryWidget->RefreshInventory(Inv);

				// 창 닫히고 키보드 포커스를 다시 인벤토리 창으로 강제 복구 - 김민성
				InventoryWidget->SetKeyboardFocus();
			}
			return; 
		}

		if (!InventoryWidget && InventoryWidgetClass)
		{
			InventoryWidget = CreateWidget<UInventoryWidget>(PC, InventoryWidgetClass);
		}

		if (InventoryWidget)
		{
			InventoryWidget->AddToViewport();
			InventoryWidget->RefreshInventory(Inv);

			bInventoryVisible = true;

			
			InventoryWidget->bIsFocusable = true;

			PC->bShowMouseCursor = true;
          
			// 포커스를 위젯으로 강제 고정
			FInputModeGameAndUI InputMode;
			InputMode.SetWidgetToFocus(InventoryWidget->TakeWidget()); 
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PC->SetInputMode(InputMode);

			
			InventoryWidget->SetUserFocus(PC);
			InventoryWidget->SetKeyboardFocus();
		}
	}
	else
	{
		if (InventoryWidget)
		{
			InventoryWidget->RemoveFromParent();
			bInventoryVisible = false;

			PC->bShowMouseCursor = false;
			FInputModeGameOnly InputMode;
			PC->SetInputMode(InputMode);
		}
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
