#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ScrollBox.h" 
#include "CraftingWindowWidget.generated.h"

UCLASS()
class IAMLEGEND_API UCraftingWindowWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> RecipeList;

	// 한 줄짜리 슬롯 위젯의 클래스 (에디터에서 할당)
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> SlotWidgetClass;

public:
	// 위젯이 생성되거나 나타날 때 호출할 함수
	UFUNCTION(BlueprintCallable)
	void RefreshRecipeList();

protected:
	virtual void NativeConstruct() override;
	
	virtual FReply NativeOnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

};