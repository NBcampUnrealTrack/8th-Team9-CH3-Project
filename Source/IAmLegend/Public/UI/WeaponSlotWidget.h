// WeaponSlotWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponSlotWidget.generated.h"

class UImage; // 전방선언 (UI 이미지)

UCLASS()
class IAMLEGEND_API UWeaponSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	UImage* Slot1_Image; // 1번 슬롯

	UPROPERTY(meta = (BindWidget))
	UImage* Slot2_Image; // 2번 슬롯

	UPROPERTY(meta = (BindWidget))
	UImage* Slot3_Image; // 3번 슬롯
};
