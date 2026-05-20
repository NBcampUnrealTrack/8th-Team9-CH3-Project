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

	// 현재 슬롯이 담당하는 레시피 데이터
	UPROPERTY()
	TObjectPtr<UCraftRecipe> TargetRecipe;
	

	// --- 결과물 및 버튼 ---
	UPROPERTY(meta = (BindWidget))
	class UImage* ResultIcon;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ResultNameText;

	UPROPERTY(meta = (BindWidget))
	class UButton* CraftButton;

	
	// 재료 위젯들이 동적으로 추가될 컨테이너 (Horizontal Box 추천)
	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* IngredientContainer;
	
	// 생성할 개별 재료 위젯의 클래스 (에디터에서 지정)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UIngredientSlotWidget> IngredientWidgetClass;
	
	// 제작 버튼 클릭 시 실행될 함수
	UFUNCTION()
	void OnCraftButtonClicked();
};