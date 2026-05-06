// TitleUIWidget.h
// 요약: 타이틀 화면 UI 출력 클래스

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TitleUIWidget.generated.h"

UCLASS()
class IAMLEGEND_API UTitleUIWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	// 위젯 바인딩(시작, 종료, FadeIn)
	UPROPERTY(meta = (BindWidget))
	class UButton* StartButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* EndButton;

	UPROPERTY(meta = (BindWidget))
	class UImage* FadeIn;

	// 애니메이션 바인딩(시작, 종료, FadeIn)
	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* StartTextHover;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* EndTextHover;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* TitleFadeIn;

private:
	// 버튼 클릭 및 호버 이벤트 함수
	UFUNCTION()
	void OnStartButtonClicked();

	UFUNCTION()
	void OnEndButtonClicked();

	UFUNCTION()
	void OnStartButtonHovered();

	UFUNCTION()
	void OnStartButtonUnhovered();

	UFUNCTION()
	void OnEndButtonHovered();

	UFUNCTION()
	void OnEndButtonUnhovered();

	// 딜레이 후 레벨 이동을 처리할 함수(FadeIn 때문에 해줘야함)
	void OpenShelterLevel();

	FTimerHandle LevelLoadTimerHandle;
};
