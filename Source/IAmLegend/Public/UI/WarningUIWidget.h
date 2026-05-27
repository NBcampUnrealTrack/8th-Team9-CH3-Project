#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WarningUIWidget.generated.h"


UCLASS()
class IAMLEGEND_API UWarningUIWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
private:
	FTimerHandle FadeTimerHandle;
	
	float FadeDuration;
	float FadeElapsed;
	
	UFUNCTION()
	void UpdateFade();
};
