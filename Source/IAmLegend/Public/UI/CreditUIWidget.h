// CreditUIWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CreditUIWidget.generated.h"

UCLASS()
class IAMLEGEND_API UCreditUIWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// ESC 키 입력 함수
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UButton* CloseButton;

private:
	UFUNCTION()
	void OnCloseClicked();
};
