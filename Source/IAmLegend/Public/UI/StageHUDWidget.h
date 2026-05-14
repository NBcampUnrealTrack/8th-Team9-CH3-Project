#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StageHUDWidget.generated.h"


UCLASS()
class IAMLEGEND_API UStageHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void UpdateRemainingTime(float RemainingSeconds);
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_RemainingTime;
	
private:
	float CurrentRemainingTime;
	
	void UpdateStageHUD();
	
	FTimerHandle UpdateTimerHandle;
};
