// CoreMinimal.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CrosshairWidget.generated.h"


UCLASS()
class IAMLEGEND_API UCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// 크로스헤어 상하좌우 파츠 바인딩
	UPROPERTY(meta = (BindWidget))
	class UImage* TopLine;

	UPROPERTY(meta = (BindWidget))
	class UImage* BottomLine;

	UPROPERTY(meta = (BindWidget))
	class UImage* LeftLine;

	UPROPERTY(meta = (BindWidget))
	class UImage* RightLine;

private:
	// 현재 벌어짐 정도와 목표 벌어짐 정도
	float CurrentSpread = 0.0f;
	float TargetSpread = 0.0f;

	// 기본 벌어짐 수치
	float BaseSpread = 5.0f;
};
