// OptionUIWidget.cpp


#include "UI/OptionUIWidget.h"

#include "Components/ComboBoxString.h"
#include "Components/Slider.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameUserSettings.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"
#include "Kismet/GameplayStatics.h"

// 볼륨 전역 변수
static float CurrentSavedVolume = 1.0f;

void UOptionUIWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeSettings();

	if (ApplyButton)
		ApplyButton->OnClicked.AddDynamic(this, &UOptionUIWidget::OnApplyClicked);

	if (CloseButton)
		CloseButton->OnClicked.AddDynamic(this, &UOptionUIWidget::OnCloseClicked);

	if (VolumeSlider)
		VolumeSlider->OnValueChanged.AddDynamic(this, &UOptionUIWidget::OnVolumeChanged);
}

void UOptionUIWidget::InitializeSettings()
{
	UGameUserSettings* Settings = GEngine->GetGameUserSettings();
	if (!Settings) return;

	// 화면 모드 초기화
	if (WindowModeComboBox)
	{
		WindowModeComboBox->ClearOptions();
		WindowModeComboBox->AddOption(TEXT("전체 화면"));
		WindowModeComboBox->AddOption(TEXT("창 모드"));

		EWindowMode::Type CurrentMode = Settings->GetFullscreenMode();
		WindowModeComboBox->SetSelectedIndex(CurrentMode == EWindowMode::Fullscreen ? 0 : 1);
	}

	// 해상도 초기화 (자주 쓰이는 해상도 예시)
	if (ResolutionComboBox)
	{
		ResolutionComboBox->ClearOptions();
		ResolutionComboBox->AddOption(TEXT("2560x1600"));
		ResolutionComboBox->AddOption(TEXT("2560x1440"));
		ResolutionComboBox->AddOption(TEXT("1920x1440"));
		ResolutionComboBox->AddOption(TEXT("1920x1080"));
		ResolutionComboBox->AddOption(TEXT("1280x720"));

		FIntPoint Res = Settings->GetScreenResolution();
		FString CurrentRes = FString::Printf(TEXT("%dx%d"), Res.X, Res.Y);
		ResolutionComboBox->SetSelectedOption(CurrentRes);
	}

	// 볼륨값
	if (VolumeSlider)
	{
		VolumeSlider->SetValue(CurrentSavedVolume);
		if (VolumeText)
		{
			VolumeText->SetText(FText::AsNumber(FMath::RoundToInt(CurrentSavedVolume * 100.0f)));
		}
	}
}

void UOptionUIWidget::OnApplyClicked()
{
	UGameUserSettings* Settings = GEngine->GetGameUserSettings();
	if (!Settings) return;

	// 해상도 적용
	if (ResolutionComboBox)
	{
		FString Selected = ResolutionComboBox->GetSelectedOption();
		FString Left, Right;
		if (Selected.Split(TEXT("x"), &Left, &Right))
		{
			Settings->SetScreenResolution(FIntPoint(FCString::Atoi(*Left), FCString::Atoi(*Right)));
		}
	}

	// 화면 모드 적용
	if (WindowModeComboBox)
	{
		int32 Index = WindowModeComboBox->GetSelectedIndex();
		Settings->SetFullscreenMode(Index == 0 ? EWindowMode::Fullscreen : EWindowMode::Windowed);
	}

	Settings->ApplySettings(false);
}

void UOptionUIWidget::OnVolumeChanged(float Value)
{
	// 변경된 값을 변수에 저장
	CurrentSavedVolume = Value;

	// 실시간 볼륨 수치 업데이트
	if (VolumeText)
	{
		VolumeText->SetText(FText::AsNumber(FMath::RoundToInt(Value * 100.0f)));
	}

	// 실제 사운드 볼륨 조절
	if (MasterSoundMix && MasterSoundClass)
	{
		UGameplayStatics::SetSoundMixClassOverride(GetWorld(), MasterSoundMix, MasterSoundClass, Value, 1.f, 1.f, true);
	}
}

void UOptionUIWidget::OnCloseClicked()
{
	RemoveFromParent();
}