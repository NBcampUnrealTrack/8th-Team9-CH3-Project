#include "Item/CraftingWindowWidget.h"
#include "Item/InventoryComponent.h"
#include "Item/CraftingSlotWidget.h"
#include "Item/CraftingManager.h"
#include "Item/IngredientSlotWidget.h"
#include "Components/Image.h"
#include "Item/CraftRecipe.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Item/ItemDataAsset.h"

void UCraftingWindowWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    bIsFocusable = true;
    
    if (DetailCraftButton)
    {
        DetailCraftButton->OnClicked.AddUniqueDynamic(this, &UCraftingWindowWidget::OnDetailCraftButtonClicked);
    }

    // 초기화: 우측 상세 화면을 비우고 목록을 채움
    DisplayRecipeDetails(nullptr);
    RefreshRecipeList();
}

FReply UCraftingWindowWidget::NativeOnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
    if (InKeyEvent.GetKey() == EKeys::Escape)
    {
       APawn* PlayerPawn = GetOwningPlayerPawn();
       if (PlayerPawn)
       {
          UInventoryComponent* InvenComp = PlayerPawn->FindComponentByClass<UInventoryComponent>();
          if (InvenComp)
          {
             InvenComp->ToggleCraftingUI(false);
             return FReply::Handled();
          }
       }
    }
    return Super::NativeOnKeyDown(MyGeometry, InKeyEvent);
}

void UCraftingWindowWidget::RefreshRecipeList()
{
    if (!RecipeList || !SlotWidgetClass) return;

    RecipeList->ClearChildren();

    APawn* PlayerPawn = GetOwningPlayerPawn();
    if (!PlayerPawn) return;

    UInventoryComponent* InvenComp = PlayerPawn->FindComponentByClass<UInventoryComponent>();
    if (InvenComp)
    {
       for (UCraftRecipe* Recipe : InvenComp->KnownRecipes)
       {
          if (!Recipe) continue;

          UCraftingSlotWidget* NewSlot = CreateWidget<UCraftingSlotWidget>(this, SlotWidgetClass);
          if (NewSlot)
          {
             
             NewSlot->SetupSlot(Recipe, this);
             RecipeList->AddChild(NewSlot);
          }
       }
    }
}

void UCraftingWindowWidget::DisplayRecipeDetails(UCraftRecipe* NewRecipe)
{
    SelectedRecipe = NewRecipe;

    // 선택된 레시피가 없거나 유효하지 않으면 우측을 완전히 숨김
    if (!SelectedRecipe || !SelectedRecipe->ResultItem)
    {
        if (DetailResultIcon) DetailResultIcon->SetVisibility(ESlateVisibility::Collapsed);
        if (DetailResultNameText) DetailResultNameText->SetText(FText::GetEmpty());
        if (DetailCraftButton) DetailCraftButton->SetVisibility(ESlateVisibility::Collapsed);
        if (DetailIngredientContainer) DetailIngredientContainer->ClearChildren();
        return;
    }

    // 결과물 기본 정보 세팅
    if (DetailResultIcon)
    {
        DetailResultIcon->SetBrushFromTexture(SelectedRecipe->ResultItem->ItemIcon);
        DetailResultIcon->SetVisibility(ESlateVisibility::Visible);
    }
    if (DetailResultNameText)
    {
        DetailResultNameText->SetText(FText::FromString(SelectedRecipe->ResultItem->ItemName));
    }
    if (DetailCraftButton)
    {
        DetailCraftButton->SetVisibility(ESlateVisibility::Visible);
    }

	// 우측 재료 레이아웃 빌드
	if (DetailIngredientContainer && IngredientWidgetClass)
	{
		DetailIngredientContainer->ClearChildren();

		
		for (const FCraftingIngredient& Ingredient : SelectedRecipe->Ingredients)
		{
			UIngredientSlotWidget* IngredientWidget = CreateWidget<UIngredientSlotWidget>(this, IngredientWidgetClass);
			if (IngredientWidget)
			{
				// Ingredient.ItemData 와 Ingredient.Quantity로 매칭!
				IngredientWidget->InitIngredient(Ingredient.ItemData, Ingredient.Quantity);
				DetailIngredientContainer->AddChild(IngredientWidget);
			}
		}
	}
}

void UCraftingWindowWidget::OnDetailCraftButtonClicked()
{
	if (SelectedRecipe != nullptr) 
	{
		APawn* PlayerPawn = GetOwningPlayerPawn();
		if (PlayerPawn != nullptr)
		{
			
			UCraftingManager* CraftManager = PlayerPawn->FindComponentByClass<UCraftingManager>();
            
			if (CraftManager != nullptr)
			{
				
				if (CraftManager->CraftItem(SelectedRecipe))
				{
					
					DisplayRecipeDetails(SelectedRecipe);
                    
					
					UInventoryComponent* InvenComp = PlayerPawn->FindComponentByClass<UInventoryComponent>();
					if (InvenComp && InvenComp->bInventoryVisible)
					{
						InvenComp->DisplayUI(true);
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("CraftingWindowWidget: 캐릭터에서 CraftingManager를 찾을 수 없습니다!"));
			}
		}
	}
}