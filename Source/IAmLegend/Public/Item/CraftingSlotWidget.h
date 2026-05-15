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
	
    
	// 결과물 아이콘
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ResultIcon;

	// 결과물 이름 또는 수량
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ResultNameText;

	// 제작 버튼
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CraftButton;

	// 제작 버튼 클릭 시 실행될 함수
	UFUNCTION()
	void OnCraftButtonClicked();
};