#include "Item/CraftingSlotWidget.h"
#include "Item/CraftRecipe.h"
#include "Item/CraftingManager.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Item/ItemDataAsset.h" 
#include "Components/HorizontalBox.h"       
#include "Item/IngredientSlotWidget.h"

void UCraftingSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (CraftButton)
	{
		// [수정] 혹시라도 이미 등록되어 있을지 모를 델리게이트를 안전하게 먼저 제거합니다.
		CraftButton->OnClicked.RemoveDynamic(this, &UCraftingSlotWidget::OnCraftButtonClicked);
       
		// 그 다음 새로 등록합니다.
		CraftButton->OnClicked.AddDynamic(this, &UCraftingSlotWidget::OnCraftButtonClicked);
	}
}
void UCraftingSlotWidget::InitSlot(UCraftRecipe* InRecipe)
{
	if (!InRecipe || !InRecipe->ResultItem) return;
	TargetRecipe = InRecipe;

	// 1. 기존에 생성되어 있던 재료 자식 위젯들을 싹 비워줍니다 (초기화)
	if (IngredientContainer)
	{
		IngredientContainer->ClearChildren();
	}

	// 2. 레시피가 가진 재료 개수만큼 반복하며 동적 생성 (1개든, 3개든, 5개든 모두 대응 가능!)
	if (IngredientContainer && IngredientWidgetClass)
	{
		for (const FCraftingIngredient& Ing : InRecipe->Ingredients)
		{
			if (!Ing.ItemData) continue;

			// 개별 재료 위젯 동적 생성
			UIngredientSlotWidget* NewIngredientWidget = CreateWidget<UIngredientSlotWidget>(this, IngredientWidgetClass);
			if (NewIngredientWidget)
			{
				// 데이터 세팅
				NewIngredientWidget->InitIngredient(Ing.ItemData, Ing.Quantity);
				// 컨테이너(Horizontal Box)에 자식으로 등록
				IngredientContainer->AddChild(NewIngredientWidget);
			}
		}
	}

	// 3. 결과물 설정 (기존과 동일)
	if (ResultIcon) ResultIcon->SetBrushFromTexture(InRecipe->ResultItem->ItemIcon);
	if (ResultNameText) ResultNameText->SetText(FText::FromString(InRecipe->ResultItem->ItemName));
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