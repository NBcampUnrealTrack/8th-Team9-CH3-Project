// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/InventoryWidget.h"
#include "Item/InventorySlotWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/TextBlock.h" 
#include "Misc/DateTime.h"
#include "GameFramework/Character.h"
#include "Item/InventoryComponent.h"
#include "Components/CanvasPanelSlot.h"

#include "Components/Image.h"
#include "Components/Button.h"
#include "Character/HanPlayerCharacter.h"
#include "BattleLogic/WeaponBase.h"

void UInventoryWidget::RefreshInventory(const TArray<FItemSlot>& Inventory)
{
	
	//인벤토리 그리드 안밀리게 고정
	if (InventoryGrid)
	{
		UCanvasPanelSlot* GridSlot = Cast<UCanvasPanelSlot>(InventoryGrid->Slot);
		if (GridSlot)
		{
			
			FVector2D FixedGridSize(716.7f, 833.f); 
			GridSlot->SetSize(FixedGridSize);
		}
	}
	
	InventoryGrid->ClearChildren();

	const int32 ColumnCount = 8;
	const int32 MaxSlotCount = 64;

	for (int32 i = 0; i < MaxSlotCount; i++)
	{
		UInventorySlotWidget* SlotWidget =
		   CreateWidget<UInventorySlotWidget>(
			  this,
			  SlotWidgetClass
		   );

		if (!SlotWidget) continue;

		
		SlotWidget->SlotIndex = i;

		// 실제 아이템 있는 슬롯만 데이터 적용
		if (Inventory.IsValidIndex(i))
		{
			SlotWidget->SetSlotData(Inventory[i]);
		}

		int32 Row = i / ColumnCount;
		int32 Col = i % ColumnCount;

		InventoryGrid->AddChildToUniformGrid(
		   SlotWidget,
		   Row,
		   Col
		);
	}
    UpdateDateTime();
}

void UInventoryWidget::UpdateDateTime()
{
    FDateTime CurrentTime = FDateTime::Now();
    
    FString DateString = CurrentTime.ToString(TEXT("%Y-%m-%d")); 
    
    EDayOfWeek DayOfWeek = CurrentTime.GetDayOfWeek();
    FString DayString = TEXT("");

    switch (DayOfWeek)
    {
    case EDayOfWeek::Monday:    DayString = TEXT("Monday"); break;
    case EDayOfWeek::Tuesday:   DayString = TEXT("Tuesday"); break;
    case EDayOfWeek::Wednesday: DayString = TEXT("Wednesday"); break;
    case EDayOfWeek::Thursday:  DayString = TEXT("Thursday"); break;
    case EDayOfWeek::Friday:    DayString = TEXT("Friday"); break;
    case EDayOfWeek::Saturday:  DayString = TEXT("Saturday"); break;
    case EDayOfWeek::Sunday:    DayString = TEXT("Sunday"); break;
    }
    

    DateString = FString::Printf(TEXT("%s %s"), *DateString, *DayString);
    FString TimeString = CurrentTime.ToString(TEXT("%H:%M"));

    if (Date)
    {
        Date->SetText(FText::FromString(DateString));
    }

    if (Time)
    {
        Time->SetText(FText::FromString(TimeString));
    }
}

FReply UInventoryWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		APlayerController* PC = GetOwningPlayer();
		if (PC)
		{
			ACharacter* PlayerChar = Cast<ACharacter>(PC->GetPawn());
			if (PlayerChar)
			{
				UInventoryComponent* InvComp = PlayerChar->FindComponentByClass<UInventoryComponent>();
				if (InvComp)
				{
					
					InvComp->DisplayUI(false);
                    
					
					return FReply::Handled();
				}
			}
		}
	}

	
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

// ---코드 추가 [김민성]
void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 중복 요청 방지 코드
	if (Btn_Unequip1) Btn_Unequip1->OnClicked.AddUniqueDynamic(this, &UInventoryWidget::OnClickUnequip1);
	if (Btn_Unequip2) Btn_Unequip2->OnClicked.AddUniqueDynamic(this, &UInventoryWidget::OnClickUnequip2);
	if (Btn_Unequip3) Btn_Unequip3->OnClicked.AddUniqueDynamic(this, &UInventoryWidget::OnClickUnequip3);
}

void UInventoryWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	AHanPlayerCharacter* PlayerCharacter = Cast<AHanPlayerCharacter>(GetOwningPlayerPawn());
	if (!PlayerCharacter) return;

	auto UpdateSlotIcon = [](UImage* TargetImage, AWeaponBase* WeaponActor)
		{
			if (TargetImage)
			{
				if (WeaponActor && WeaponActor->WeaponIcon)
				{
					TargetImage->SetBrushFromTexture(WeaponActor->WeaponIcon);
					TargetImage->SetVisibility(ESlateVisibility::HitTestInvisible);
				}
				else
				{
					TargetImage->SetVisibility(ESlateVisibility::Hidden);
				}
			}
		};

	const TMap<EWeaponSlot, AWeaponBase*>& ActiveWeaponSlots = PlayerCharacter->GetWeaponSlots();

	AWeaponBase* MeleeWeapon = ActiveWeaponSlots.Contains(EWeaponSlot::Melee) ? ActiveWeaponSlots[EWeaponSlot::Melee] : nullptr;
	UpdateSlotIcon(InvWeaponImage_1, MeleeWeapon);

	AWeaponBase* RangedWeapon = ActiveWeaponSlots.Contains(EWeaponSlot::Ranged) ? ActiveWeaponSlots[EWeaponSlot::Ranged] : nullptr;
	UpdateSlotIcon(InvWeaponImage_2, RangedWeapon);

	AWeaponBase* DaggerWeapon = ActiveWeaponSlots.Contains(EWeaponSlot::Dagger) ? ActiveWeaponSlots[EWeaponSlot::Dagger] : nullptr;
	UpdateSlotIcon(InvWeaponImage_3, DaggerWeapon);
}

void UInventoryWidget::OnClickUnequip1()
{
	AHanPlayerCharacter* PlayerCharacter = Cast<AHanPlayerCharacter>(GetOwningPlayerPawn());
	if (PlayerCharacter)
	{
		PlayerCharacter->UnEquipWeapon(EWeaponSlot::Melee);

		UInventoryComponent* InvComp = PlayerCharacter->FindComponentByClass<UInventoryComponent>();
		if (InvComp) RefreshInventory(InvComp->GetActualInventory());
	}
}

void UInventoryWidget::OnClickUnequip2()
{
	AHanPlayerCharacter* PlayerCharacter = Cast<AHanPlayerCharacter>(GetOwningPlayerPawn());
	if (PlayerCharacter)
	{
		PlayerCharacter->UnEquipWeapon(EWeaponSlot::Ranged);

		UInventoryComponent* InvComp = PlayerCharacter->FindComponentByClass<UInventoryComponent>();
		if (InvComp) RefreshInventory(InvComp->GetActualInventory());
	}
}

void UInventoryWidget::OnClickUnequip3()
{
	AHanPlayerCharacter* PlayerCharacter = Cast<AHanPlayerCharacter>(GetOwningPlayerPawn());
	if (PlayerCharacter)
	{
		PlayerCharacter->UnEquipWeapon(EWeaponSlot::Dagger);

		UInventoryComponent* InvComp = PlayerCharacter->FindComponentByClass<UInventoryComponent>();
		if (InvComp) RefreshInventory(InvComp->GetActualInventory());
	}
}