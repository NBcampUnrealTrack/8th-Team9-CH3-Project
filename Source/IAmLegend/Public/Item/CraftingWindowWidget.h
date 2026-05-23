#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ScrollBox.h" 
#include "Item/CraftRecipe.h"
#include "CraftingWindowWidget.generated.h"

class UImage;
class UTextBlock;
class UButton;
class UHorizontalBox;
class UIngredientSlotWidget;
class UCraftingSlotWidget;

UCLASS()
class IAMLEGEND_API UCraftingWindowWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// --- 좌측 영역 ---
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> RecipeList;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UCraftingSlotWidget> SlotWidgetClass;

	// --- 우측 상세 정보 영역 ---
	UPROPERTY(meta = (BindWidget))
	UImage* DetailResultIcon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DetailResultNameText;

	UPROPERTY(meta = (BindWidget))
	UButton* DetailCraftButton;
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* DetailIngredientContainer;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UIngredientSlotWidget> IngredientWidgetClass;

	// 현재 선택된 레시피 데이터 저장
	UPROPERTY()
	TObjectPtr<UCraftRecipe> SelectedRecipe;

public:
	// 목록 전체 갱신
	UFUNCTION(BlueprintCallable)
	void RefreshRecipeList();

	// 특정 레시피가 선택되었을 때 우측 상세창을 갱신하는 함수
	void DisplayRecipeDetails(UCraftRecipe* NewRecipe);

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

	// 우측 제작 버튼 클릭 이벤트
	UFUNCTION()
	void OnDetailCraftButtonClicked();
};