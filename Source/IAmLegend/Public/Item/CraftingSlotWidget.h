#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/CraftRecipe.h"
#include "CraftingSlotWidget.generated.h"

class UImage;
class UTextBlock;
class UButton;
class UCraftingWindowWidget;

UCLASS()
class IAMLEGEND_API UCraftingSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	void SetupSlot(UCraftRecipe* InRecipe, UCraftingWindowWidget* InParent);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY()
	TObjectPtr<UCraftRecipe> TargetRecipe;

	// 나를 소유한 최상위 윈도우 캐싱
	UPROPERTY()
	TObjectPtr<UCraftingWindowWidget> ParentWindow;
    
	UPROPERTY(meta = (BindWidget))
	UImage* ResultIcon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ResultNameText;

	UPROPERTY(meta = (BindWidget))
	UButton* SlotSelectButton;

	UFUNCTION()
	void OnSlotSelectButtonClicked();
};