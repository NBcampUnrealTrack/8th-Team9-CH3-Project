#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/CraftRecipe.h"
#include "CraftingSlotWidget.generated.h"

class UImage;
class UTextBlock;
class UButton;

UCLASS()
class IAMLEGEND_API UCraftingSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 데이터를 위젯에 전달하는 핵심 함수
	void InitSlot(UCraftRecipe* InRecipe);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY()
	TObjectPtr<UCraftRecipe> TargetRecipe;

	// --- 재료 1 ---
	UPROPERTY(meta = (BindWidget))
	class UImage* Ingredient_1_Icon;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Ingredient_1_Count;

	// --- 재료 2 ---
	UPROPERTY(meta = (BindWidget))
	class UImage* Ingredient_2_Icon;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Ingredient_2_Count;

	// --- 결과물 및 버튼 ---
	UPROPERTY(meta = (BindWidget))
	class UImage* ResultIcon;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ResultNameText;
	UPROPERTY(meta = (BindWidget))
	class UButton* CraftButton;
	

	// 제작 버튼 클릭 시 실행될 함수
	UFUNCTION()
	void OnCraftButtonClicked();
};