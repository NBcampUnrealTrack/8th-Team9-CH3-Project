// WeaponInstallationWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponInstallationWidget.generated.h"

UCLASS()
class IAMLEGEND_API UWeaponInstallationWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// UI 위젯 바인딩
	UPROPERTY(meta = (BindWidget))
	class UImage* WeaponImage;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CurrentAmmoText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TotalAmmoText;

	UPROPERTY(meta = (BindWidget))
	class UImage* GrenadeImage;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* GrenadeCountText;
};
