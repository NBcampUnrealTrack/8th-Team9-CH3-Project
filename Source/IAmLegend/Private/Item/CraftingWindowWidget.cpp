#include "Item/CraftingWindowWidget.h"
#include "Item/CraftingManager.h"
#include "Item/InventoryComponent.h"
#include "Item/CraftingSlotWidget.h" // 슬롯 위젯 헤더

void UCraftingWindowWidget::NativeConstruct()
{
	Super::NativeConstruct();
	RefreshRecipeList(); // 위젯이 켜질 때 목록 갱신
}

void UCraftingWindowWidget::RefreshRecipeList()
{
	if (!RecipeList || !SlotWidgetClass) return;

	RecipeList->ClearChildren();

	APawn* PlayerPawn = GetOwningPlayerPawn();
	if (!PlayerPawn) return;

	// 인벤토리 컴포넌트를 먼저 검색 레시피가 여기 있음
	UInventoryComponent* InvenComp = PlayerPawn->FindComponentByClass<UInventoryComponent>();
    
	if (InvenComp)
	{
		// 인벤토리가 가진 KnownRecipes를 사용
		for (UCraftRecipe* Recipe : InvenComp->KnownRecipes)
		{
			UCraftingSlotWidget* NewSlot = CreateWidget<UCraftingSlotWidget>(this, SlotWidgetClass);
			if (NewSlot)
			{
				NewSlot->InitSlot(Recipe);
				RecipeList->AddChild(NewSlot);
			}
		}
	}
}