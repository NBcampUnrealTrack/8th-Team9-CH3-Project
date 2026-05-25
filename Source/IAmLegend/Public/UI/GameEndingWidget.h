// GameEndingWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameEndingWidget.generated.h"

UCLASS()
class IAMLEGEND_API UGameEndingWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	// 점수 표시 텍스트
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TotalScore;

	// 죽인 좀비 수 표시 텍스트
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* KillCount;

	// 타이틀 화면으로 돌아가는 버튼
	UPROPERTY(meta = (BindWidget))
	class UButton* TitleButton;

private:
	// 타이틀 이동 버튼 클릭 이벤트
	UFUNCTION()
	void OnTitleButtonClicked();
};
