// OptionUIWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionUIWidget.generated.h"

UCLASS()
class IAMLEGEND_API UOptionUIWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* VolumeText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Audio")
	class USoundMix* MasterSoundMix;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Audio")
	class USoundClass* MasterSoundClass;

	// ESC 키 입력 함수
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	
protected:
	virtual void NativeConstruct() override;

	// 해상도 & 화면 모드
	UPROPERTY(meta = (BindWidget))
	class UComboBoxString* ResolutionComboBox;

	UPROPERTY(meta = (BindWidget))
	class UComboBoxString* WindowModeComboBox;

	// 사운드
	UPROPERTY(meta = (BindWidget))
	class USlider* VolumeSlider;

	// 적용 버튼
	UPROPERTY(meta = (BindWidget))
	class UButton* ApplyButton;

	// 닫기 버튼
	UPROPERTY(meta = (BindWidget))
	class UButton* CloseButton;

private:
	// 초기 설정 로드
	void InitializeSettings();

	UFUNCTION()
	void OnApplyClicked();

	UFUNCTION()
	void OnCloseClicked();

	// 볼륨 변경 시 로그 출력 (실제 사운드 클래스 믹스에 연결 가능)
	UFUNCTION()
	void OnVolumeChanged(float Value);
};
