#include "Item/CraftingSlotWidget.h"
#include "Item/CraftRecipe.h"
#include "Item/CraftingManager.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Item/ItemDataAsset.h" 

void UCraftingSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (CraftButton)
	{
		CraftButton->OnClicked.AddDynamic(this, &UCraftingSlotWidget::OnCraftButtonClicked);
	}
}

void UCraftingSlotWidget::InitSlot(UCraftRecipe* InRecipe)
{
	if (!InRecipe || !InRecipe->ResultItem) return;
	TargetRecipe = InRecipe;

	// 모든 재료 UI를 일단 숨김 처리
	Ingredient_1_Icon->SetVisibility(ESlateVisibility::Collapsed);
	Ingredient_1_Count->SetVisibility(ESlateVisibility::Collapsed);
	Ingredient_2_Icon->SetVisibility(ESlateVisibility::Collapsed);
	Ingredient_2_Count->SetVisibility(ESlateVisibility::Collapsed);

	// 1. 재료 데이터 채우기
	for (int32 i = 0; i < InRecipe->Ingredients.Num(); ++i)
	{
		const FCraftingIngredient& Ing = InRecipe->Ingredients[i];
		if (!Ing.ItemData) continue;

		if (i == 0) // 첫 번째 재료
		{
			Ingredient_1_Icon->SetBrushFromTexture(Ing.ItemData->ItemIcon);
			Ingredient_1_Count->SetText(FText::AsNumber(Ing.Quantity));
			Ingredient_1_Icon->SetVisibility(ESlateVisibility::Visible);
			Ingredient_1_Count->SetVisibility(ESlateVisibility::Visible);
		}
		else if (i == 1) // 두 번째 재료
		{
			Ingredient_2_Icon->SetBrushFromTexture(Ing.ItemData->ItemIcon);
			Ingredient_2_Count->SetText(FText::AsNumber(Ing.Quantity));
			Ingredient_2_Icon->SetVisibility(ESlateVisibility::Visible);
			Ingredient_2_Count->SetVisibility(ESlateVisibility::Visible);
		}
	}

	// 2. 결과물 설정
	ResultIcon->SetBrushFromTexture(InRecipe->ResultItem->ItemIcon);
	ResultNameText->SetText(FText::FromString(InRecipe->ResultItem->ItemName));
}

void UCraftingSlotWidget::OnCraftButtonClicked()
{
	if (!TargetRecipe) return;

	// 캐릭터를 가져와서 CraftingManager를 찾음
	APawn* PlayerPawn = GetOwningPlayerPawn();
	if (!PlayerPawn) return;

	UCraftingManager* CraftManager = PlayerPawn->FindComponentByClass<UCraftingManager>();
	if (CraftManager)
	{
		// 실제 조합 시도
		bool bSuccess = CraftManager->CraftItem(TargetRecipe);
        
		if (bSuccess)
		{
			UE_LOG(LogTemp, Log, TEXT("UI: 조합 성공!"));
		
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UI: 재료 부족으로 조합 실패"));
		}
	}
}