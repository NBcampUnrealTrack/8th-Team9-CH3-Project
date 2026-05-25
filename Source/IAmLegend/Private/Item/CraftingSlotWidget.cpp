#include "Item/CraftingSlotWidget.h"
#include "Item/CraftingWindowWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Item/ItemDataAsset.h"

void UCraftingSlotWidget::SetupSlot(UCraftRecipe* InRecipe, UCraftingWindowWidget* InParent)
{
	TargetRecipe = InRecipe;
	ParentWindow = InParent; 

	if (!TargetRecipe || !TargetRecipe->ResultItem) return;

	if (ResultIcon)
	{
		ResultIcon->SetBrushFromTexture(TargetRecipe->ResultItem->ItemIcon);
	}

	if (ResultNameText)
	{
		ResultNameText->SetText(FText::FromString(TargetRecipe->ResultItem->ItemName));
	}
}

void UCraftingSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (SlotSelectButton)
	{
		SlotSelectButton->OnClicked.AddUniqueDynamic(this, &UCraftingSlotWidget::OnSlotSelectButtonClicked);
	}
}

void UCraftingSlotWidget::OnSlotSelectButtonClicked()
{
	
	if (ParentWindow && TargetRecipe)
	{
		ParentWindow->DisplayRecipeDetails(TargetRecipe);
	}
}