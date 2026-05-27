// StealthWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StealthWidget.generated.h"

class UImage; // 전방 선언
class UMaterialInstanceDynamic; // 전방 선언

UCLASS()
class IAMLEGEND_API UStealthWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	UImage* QKeyIcon;

	UPROPERTY(meta = (BindWidget))
	UImage* StealthIcon;

private:
	// 머티리얼의 파라미터 실시간 조작 변수
	UPROPERTY()
	UMaterialInstanceDynamic* QKeyMaterial;

	UPROPERTY()
	UMaterialInstanceDynamic* StealthMaterial;
};
