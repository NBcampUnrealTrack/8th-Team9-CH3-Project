#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TestUserWidget.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class IAMLEGEND_API UTestUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_GameStart;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_Shelter;
	
	
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnGameStartClicked();
};
