// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/InventoryComponent.h"
#include "ItemActionPopUpWidget.generated.h"

/**
 * 
 */
UCLASS()
class IAMLEGEND_API UItemActionPopUpWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(meta = (BindWidget))
	class UButton* UseButton;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ButtonText;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemNameText;

	
	FItemSlot TargetSlotData;
	
	// 팝업을 띄울 때 슬롯의 인덱스도 같이 넘겨받아 저장할 변수
	int32 TargetIndex = INDEX_NONE;
	
	void SetupPopup(const FItemSlot& InSlotData, int32 InIndex);
	void ShowOnlyName(const FItemSlot& InSlotData);
	

protected:
	
	virtual void NativeConstruct() override; 

	// 버튼 클릭 시 호출될 함수
	UFUNCTION()
	void OnUseButtonClicked();
};
