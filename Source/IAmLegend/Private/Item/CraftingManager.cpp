// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/CraftingManager.h"
#include "Item/InventoryComponent.h"
#include "Item/CraftRecipe.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UCraftingManager::UCraftingManager()
{
	PrimaryComponentTick.bCanEverTick = false;

}


void UCraftingManager::BeginPlay()
{
	Super::BeginPlay();

	InventoryComp = GetOwner()->FindComponentByClass<UInventoryComponent>();

	if (InventoryComp)
	{
		// 매니저의 기본 레시피들을 인벤토리의 '습득 리스트'에 등록
		for (UCraftRecipe* DefaultRecipe : DefaultRecipes)
		{
			if (DefaultRecipe)
			{
				InventoryComp->KnownRecipes.AddUnique(DefaultRecipe);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("CraftingManager: InventoryComponent를 찾을 수 없습니다!"));
	}
}


bool UCraftingManager::CraftItem(UCraftRecipe* Recipe)
{
	// 조합 가능한지 먼저 확인
	if (!CanCraft(Recipe))
	{
		UE_LOG(LogTemp, Warning, TEXT("조합 실패: 재료가 부족합니다."));
		return false;
	}

	// 재료 소모 (실제 데이터 차감)
	for (const FCraftingIngredient& Ingredient : Recipe->Ingredients)
	{
		
		InventoryComp->RemoveItemQuantity(Ingredient.ItemData, Ingredient.Quantity);
	}

	// 결과물 생성 및 인벤토리 추가
	if (Recipe->ResultItem)
	{
	
		for (int32 i = 0; i < Recipe->ResultQuantity; ++i)
		{
			InventoryComp->AddItem(Recipe->ResultItem);
		}

		UE_LOG(LogTemp, Log, TEXT("조합 성공: %s %d개 생성됨"), 
			*Recipe->ResultItem->ItemName, Recipe->ResultQuantity);

		
		return true;
	}

	return false;
}

bool UCraftingManager::CanCraft(UCraftRecipe* Recipe) const
{
	if (!Recipe || !InventoryComp) return false;

	// 레시피에 등록된 모든 재료를 순회하며 검사
	for (const FCraftingIngredient& Ingredient : Recipe->Ingredients)
	{
		if (!Ingredient.ItemData) continue;

		// 인벤토리에 해당 아이템이 필요한 개수만큼 있는지 확인
		int32 CurrentAmount = InventoryComp->GetItemQuantity(Ingredient.ItemData);
        
		if (CurrentAmount < Ingredient.Quantity)
		{
			return false; 
		}
	}

	return true; 
}


void UCraftingManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// GetRecipesToDisplay 구현 (인벤토리의 레시피를 반환)
TArray<UCraftRecipe*> UCraftingManager::GetRecipesToDisplay() const
{
	if (InventoryComp)
	{
		return InventoryComp->KnownRecipes;
	}
	return TArray<UCraftRecipe*>();
}
