// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CraftNotificationWidget.generated.h"

/**
 * 
 */

class UTextBlock;
class UWidgetAnimation;
class UButton;

UCLASS()
class IAMLEGEND_API UCraftNotificationWidget : public UUserWidget
{
	GENERATED_BODY()
	
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NotificationText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackgroundCloseButton;

public:
	void ShowNotification(const FText& Message, const FLinearColor& TextColor);

protected:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnBackgroundClicked();
};
