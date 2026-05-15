// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CraftRecipe.h"
#include "CraftingManager.generated.h"

class UInventoryComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class IAMLEGEND_API UCraftingManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCraftingManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	bool CraftItem(UCraftRecipe* Recipe);

	//현재 인벤토리의 재료로 해당 레시피가 조합 가능한지 확인 
	UFUNCTION(BlueprintPure, Category = "Crafting")
	bool CanCraft(UCraftRecipe* Recipe) const;
	
	// 시작할 때 기본으로 넣어줄 레시피 (에디터 설정용)
	UPROPERTY(EditAnywhere, Category = "Crafting")
	TArray<TObjectPtr<UCraftRecipe>> DefaultRecipes;

	// UI에서 리스트를 그릴 때 호출할 Getter
	UFUNCTION(BlueprintCallable, Category = "Crafting")
	TArray<UCraftRecipe*> GetRecipesToDisplay() const;

private:
	// 소유자 캐릭터로부터 인벤토리 컴포넌트를 가져와 저장할 변수
	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventoryComp;
	
	UPROPERTY(EditAnywhere, Category = "Crafting Data")
	TArray<TObjectPtr<UCraftRecipe>> AllAvailableRecipes;
	
};
