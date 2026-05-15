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
	if (!InRecipe) return;

	TargetRecipe = InRecipe;

	
	if (ResultIcon && TargetRecipe->ResultItem)
	{
		ResultIcon->SetBrushFromTexture(TargetRecipe->ResultItem->ItemIcon);
	}

	
	if (ResultNameText)
	{
		FString DisplayString = FString::Printf(TEXT("%s (%d)"), 
			*TargetRecipe->ResultItem->ItemName, TargetRecipe->ResultQuantity);
		ResultNameText->SetText(FText::FromString(DisplayString));
	}
    
	
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