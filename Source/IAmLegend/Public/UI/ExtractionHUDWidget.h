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
	
	void ShowEscapingZone();
	void HideEscapingZone();
	void SuccessEscape();
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_RemainingTime;

	
	
private:
	void UpdateEscapingTimer();
	
	float RemainingEscapingTime;
	
};
