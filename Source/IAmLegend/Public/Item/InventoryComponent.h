// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class UItemDataAsset;
class UInventoryWidget;
class UCraftRecipe;

USTRUCT(BlueprintType)
struct FItemSlot
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UItemDataAsset> ItemData = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Quantity = 0;

	FItemSlot() {}
	FItemSlot(UItemDataAsset* InData, int32 InQuantity) 
		: ItemData(InData), Quantity(InQuantity) {}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class IAMLEGEND_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public: 
	UInventoryComponent();
    
	UFUNCTION(BlueprintCallable)
	bool AddItem(UItemDataAsset* NewItem);
    
	UFUNCTION(BlueprintCallable)
	void UseItem(int32 Index);

	// 현재 인벤토리의 모든 아이템과 카테고리별 수량을 출력하는 함수
	UFUNCTION(BlueprintCallable)
	void ShowInventory();
	
	UFUNCTION(BlueprintCallable)
	void ShowInventoryOnScreen();
	
	UFUNCTION()
	void DisplayUI();
	
	UFUNCTION()
	int32 GetItemQuantity(UItemDataAsset* TargetItem);
	
	UFUNCTION()
	void RemoveItemQuantity(UItemDataAsset* TargetItem, int32 Amount);
	
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> CraftingWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> CraftingWidget;

	// 제작 전용 UI 토글 함수
	void ToggleCraftingUI(bool bShow);
	
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FItemSlot> Inventory;
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TArray<FItemSlot>& GetActualInventory();
	
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	TArray<TObjectPtr<UCraftRecipe>> KnownRecipes;
       
protected:
	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;
	
	UPROPERTY()
	TObjectPtr<UInventoryWidget> InventoryWidget;
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UInventoryWidget> InventoryWidgetClass;
	
	UPROPERTY()
	TArray<FItemSlot> CurrentStageAcquiredItems;
	
public:
	UFUNCTION()
	TArray<FItemSlot> GetItemList() const { return CurrentStageAcquiredItems; }
	
	virtual void BeginPlay() override;
};
