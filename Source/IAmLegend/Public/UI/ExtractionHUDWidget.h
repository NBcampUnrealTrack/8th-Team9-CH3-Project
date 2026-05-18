#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ExtractionHUDWidget.generated.h"


UCLASS()
class IAMLEGEND_API UExtractionHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
		
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_RemainingTime;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* PB_RemainingTime;

	
	
private:
	//탈출 지점 UI 업데이트
	void UpdateExtractionHUD();

	UPROPERTY()
	class AEscapingPoint* EscapingPoint;
	float MaxEscapingDuration;
	
	float RemainingEscapingTime;
	
	FTimerHandle UpdateTimerHandle;
	
};
