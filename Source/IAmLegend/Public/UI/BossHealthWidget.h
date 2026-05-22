// BossHealthWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BossHealthWidget.generated.h"

// 전방 선언
class UProgressBar;
class UImage;
class UTextBlock;

UCLASS()
class IAMLEGEND_API UBossHealthWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 보스 체력바
	UPROPERTY(meta = (BindWidget))
	UProgressBar* BossProgressBar;

	// 보스 체력바 테두리
	UPROPERTY(meta = (BindWidget))
	UImage* BossFrameImage;

	// 보스 이름
	UPROPERTY(meta = (BindWidget))
	UTextBlock* BossNameText;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	// const float BossDetectDistance = 1000.f;

	// UI 한 번에 숨기거나 켜는 함수
	void SetHealthUIVisibility(ESlateVisibility InVisibility);

	bool bHasEncountered = false;
};
