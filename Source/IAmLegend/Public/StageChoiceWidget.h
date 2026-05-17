// StageChoiceWidget.h
// 요약: 스테이지 선택 UI 화면 출력

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StageChoiceWidget.generated.h"


UCLASS()
class IAMLEGEND_API UStageChoiceWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	// 버튼 바인딩
	UPROPERTY(meta = (BindWidget))
	class UButton* Hospital;

	UPROPERTY(meta = (BindWidget))
	class UButton* School;

	UPROPERTY(meta = (BindWidget))
	class UButton* Police;

	UPROPERTY(meta = (BindWidget))
	class UButton* Back;

	UPROPERTY(meta = (BindWidget))
	class UButton* Boss;

private:
	UFUNCTION()
	void OnHospitalClicked();

	UFUNCTION()
	void OnSchoolClicked();

	UFUNCTION()
	void OnPoliceClicked();

	UFUNCTION()
	void OnBackClicked();

	UFUNCTION()
	void OnBossClicked();
};
