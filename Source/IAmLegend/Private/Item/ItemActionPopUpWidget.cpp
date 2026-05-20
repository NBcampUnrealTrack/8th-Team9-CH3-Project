// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemActionPopUpWidget.h"
#include "Components/Button.h"
#include "Item/UseItemDataAsset.h"
#include "Character/HanPlayerCharacter.h" // 캐릭터 캐스팅용 추가
#include "Item/InventoryComponent.h"

void UItemActionPopUpWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UseButton)
	{
		UseButton->OnClicked.AddUniqueDynamic(this, &UItemActionPopUpWidget::OnUseButtonClicked);
	}
}

void UItemActionPopUpWidget::OnUseButtonClicked()
{
	// 인덱스가 유효하고 아이템 데이터가 있는지 확인
	if (TargetIndex != INDEX_NONE && TargetSlotData.ItemData)
	{
		// 1. 로컬 플레이어 캐릭터 가져오기
		APlayerController* PC = GetOwningPlayer();
		if (PC)
		{
			AHanPlayerCharacter* PlayerChar = Cast<AHanPlayerCharacter>(PC->GetPawn());
			if (PlayerChar)
			{
				// 2. 캐릭터가 가진 인벤토리 컴포넌트 컴포넌트 찾아오기
				// (만약 인벤토리 컴포넌트를 반환하는 Getter 함수가 있다면 그걸 쓰셔도 좋습니다)
				UInventoryComponent* InvComp = Cast<UInventoryComponent>(PlayerChar->GetComponentByClass(UInventoryComponent::StaticClass()));
                
				if (InvComp)
				{
					// 3. 실제 인벤토리 컴포넌트의 사용 로직 실행!
					InvComp->UseItem(TargetIndex);

					// 4. 아이템을 썼으니 화면의 인벤토리 UI도 최신 수량으로 새로고침
					InvComp->ShowInventory();
				}
			}
		}

		// 사용 후 팝업 닫기
		RemoveFromParent();
	}
}