// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EscapeTimerWidget.generated.h"

/**
 * 
 */
UCLASS()
class IAMLEGEND_API UEscapeTimerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	void ShowEscapingZone();
	void HideEscapingZone();
	void SuccessEscape();
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_RemainingTime;
	UPROPERTY(meta = (BindWidget))
	
	
private:
	void UpdateEscapingTimer();
	
	float RemainingEscapingTime;
	
};
