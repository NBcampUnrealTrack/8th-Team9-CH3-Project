// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/InventoryComponent.h"
#include "InventorySlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class IAMLEGEND_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta=(BindWidget))
	class UImage* ItemIcon;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Quantity;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* SlotButton;

	// 에디터에서 할당할 팝업 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UItemActionPopUpWidget> PopupWidgetClass;
	
	
	UPROPERTY(BlueprintReadWrite, Category = "Inventory Slot")
	int32 SlotIndex = INDEX_NONE;
	
	void SetSlotData(const FItemSlot& SlotData);
	
	void ClearSlot();
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnSlotClicked();
private:
	// 현재 슬롯이 들고 있는 데이터 백업용
	FItemSlot CurrentSlotData;
};
