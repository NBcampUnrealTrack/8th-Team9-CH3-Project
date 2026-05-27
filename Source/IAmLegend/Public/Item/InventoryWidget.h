// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryComponent.h"
#include "BattleLogic/Attachment/AttachmentDataAsset.h"
#include "InventoryWidget.generated.h"

/**
 * 
 */

class UUniformGridPanel;
class UInventorySlotWidget;
class UInventorySlotWidget;
class UTextBlock;
class UImage;
class UButton;
class URangedAttachmentComponent;

UCLASS()
class IAMLEGEND_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void RefreshInventory(const TArray<FItemSlot>& Inventory);
    
    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateDateTime();
	
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

protected:

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	UImage* InvWeaponImage_1;

	UPROPERTY(meta = (BindWidget))
	UImage* InvWeaponImage_2;

	UPROPERTY(meta = (BindWidget))
	UImage* InvWeaponImage_3;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Unequip1;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Unequip2;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Unequip3;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Date;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* Time;
    
	UPROPERTY(meta = (BindWidget))
	UUniformGridPanel* InventoryGrid;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UInventorySlotWidget> SlotWidgetClass;

	// 부착물 관련 UI
	UPROPERTY(meta = (BindWidget))
	UButton* SightButton;

	UPROPERTY(meta = (BindWidget))
	UButton* StockButton;

	UPROPERTY(meta = (BindWidget))
	UButton* MuzzleButton;

	UPROPERTY(meta = (BindWidget))
	UButton* MagazineButton;

	UPROPERTY(meta = (BindWidget))
	UButton* GripButton;

	UPROPERTY(meta = (BindWidget))
	UImage* SightImage;

	UPROPERTY(meta = (BindWidget))
	UImage* StockImage;

	UPROPERTY(meta = (BindWidget))
	UImage* MuzzleImage;

	UPROPERTY(meta = (BindWidget))
	UImage* MagazineImage;

	UPROPERTY(meta = (BindWidget))
	UImage* GripImage;
	
	UPROPERTY(BlueprintReadOnly)
	TMap<EAttachmentSlot, UImage*> AttachmentSlotImages;

	UPROPERTY(BlueprintReadOnly)
	TMap<EAttachmentSlot, UButton*> AttachmentSlotButtons;

	UPROPERTY(meta = (BindWidget))
	UImage* AttachmentsWeaponImage;

	TWeakObjectPtr<URangedAttachmentComponent> CurrentAttachmentComponent;

private:
	UFUNCTION()
	void OnClickUnequip1();

	UFUNCTION()
	void OnClickUnequip2();

	UFUNCTION()
	void OnClickUnequip3();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Attachments")
	void OnClickAttachment(EAttachmentSlot AttachmentSlot);

	UFUNCTION()
	void UpdateAttachmentUI(URangedAttachmentComponent* AttachmentComp);

	UFUNCTION()
	void UpdateAttachmentComponent();
};
