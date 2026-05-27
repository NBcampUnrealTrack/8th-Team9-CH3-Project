// TitleUIWidget.cpp
// 요약: 타이틀 화면 UI 출력 클래스


#include "TitleUIWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Animation/WidgetAnimation.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"
#include "Gamemode/MainGameModeBase.h" // 게임모드 추가
#include "UI/MainHUD.h"

void UTitleUIWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 델리게이트 바인딩: UI 이벤트(클릭, 마우스 오버 등)를 C++ 함수에 연결
	if (StartButton)
	{
		StartButton->OnClicked.AddDynamic(this, &UTitleUIWidget::OnStartButtonClicked);
		StartButton->OnHovered.AddDynamic(this, &UTitleUIWidget::OnStartButtonHovered);
		StartButton->OnUnhovered.AddDynamic(this, &UTitleUIWidget::OnStartButtonUnhovered);
	}

	if (EndButton)
	{
		EndButton->OnClicked.AddDynamic(this, &UTitleUIWidget::OnEndButtonClicked);
		EndButton->OnHovered.AddDynamic(this, &UTitleUIWidget::OnEndButtonHovered);
		EndButton->OnUnhovered.AddDynamic(this, &UTitleUIWidget::OnEndButtonUnhovered);
	}

	if (OptionsButton)
	{
		OptionsButton->OnClicked.AddDynamic(this, &UTitleUIWidget::OnOptionsButtonClicked);
		OptionsButton->OnHovered.AddDynamic(this, &UTitleUIWidget::OnOptionsButtonHovered);
		OptionsButton->OnUnhovered.AddDynamic(this, &UTitleUIWidget::OnOptionsButtonUnhovered);
	}

	if (CreditButton)
	{
		CreditButton->OnClicked.AddDynamic(this, &UTitleUIWidget::OnCreditButtonClicked);
		CreditButton->OnHovered.AddDynamic(this, &UTitleUIWidget::OnCreditButtonHovered);
		CreditButton->OnUnhovered.AddDynamic(this, &UTitleUIWidget::OnCreditButtonUnhovered);
	}
}

void UTitleUIWidget::OnStartButtonClicked()
{
	// FadeIn 이미지를 보이게 설정하고 애니메이션 재생
	if (FadeIn)
	{
		FadeIn->SetVisibility(ESlateVisibility::Visible);
	}

	// FadeIn 애니메이션 재생
	if (TitleFadeIn)
	{
		PlayAnimation(TitleFadeIn);
	}

	// [추가] StartGame() 함수 호출 + FadeIn 재생 시간만큼 대기 후 진행
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()
		{
			// AMainGameModeBase* GameMode = Cast<AMainGameModeBase>(UGameplayStatics::GetGameMode(this));
			// if (GameMode)
			// {
			// 	GameMode->StartGame();
			// }
			if (StoryUIClass)
			{
				UUserWidget* StoryWidget = CreateWidget<UUserWidget>(GetWorld(), StoryUIClass);
				if (StoryWidget)
				{
					StoryWidget->AddToViewport(); // 만화 화면 띄우기
					this->RemoveFromParent();     // 타이틀 화면 지우기
				}
			}
		}), 1.0f, false); // 애니메이션 길이에 맞게 1.0f 세팅
}

// 종료 함수
void UTitleUIWidget::OnEndButtonClicked()
{
	// 게임 종료
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, false);
}

// 옵션 함수
void UTitleUIWidget::OnOptionsButtonClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		AMainHUD* HUD = Cast<AMainHUD>(PC->GetHUD());
		if (HUD)
		{
			HUD->ShowOptionHUD(); // 옵션 출력
		}
	}
}

// 크레딧 함수
void UTitleUIWidget::OnCreditButtonClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (PC)
	{
		AMainHUD* HUD = Cast<AMainHUD>(PC->GetHUD());
		if (HUD)
		{
			HUD->ShowCreditHUD(); // 크레딧 출력
		}
	}
}

// 시작 버튼에 있으면 텍스트 애니메이션 재생
void UTitleUIWidget::OnStartButtonHovered()
{
	if (StartTextHover)
	{
		PlayAnimation(StartTextHover);
	}
}

// 시작 버튼에 벗어나면 텍스트 애니메이션 역방향으로 재생
void UTitleUIWidget::OnStartButtonUnhovered()
{
	if (StartTextHover)
	{
		PlayAnimation(StartTextHover, 0.0f, 1, EUMGSequencePlayMode::Reverse, 1.0f);
	}
}

// 종료 버튼에 있으면 텍스트 애니메이션 재생
void UTitleUIWidget::OnEndButtonHovered()
{
	if (EndTextHover)
	{
		PlayAnimation(EndTextHover);
	}
}

// 종료 버튼에서 벗어나면 텍스트 애니메이션 역방향으로 재생
void UTitleUIWidget::OnEndButtonUnhovered()
{
	if (EndTextHover)
	{
		PlayAnimation(EndTextHover, 0.0f, 1, EUMGSequencePlayMode::Reverse, 1.0f);
	}
}

// 옵션 버튼에 있으면 텍스트 애니메이션 재생
void UTitleUIWidget::OnOptionsButtonHovered()
{
	if (OptionsTextHover)
	{
		PlayAnimation(OptionsTextHover);
	}
}

// 옵션 버튼에서 벗어나면 텍스트 애니메이션 역방향으로 재생
void UTitleUIWidget::OnOptionsButtonUnhovered()
{
	if (OptionsTextHover)
	{
		PlayAnimation(OptionsTextHover, 0.0f, 1, EUMGSequencePlayMode::Reverse, 1.0f);
	}
}

// 크레딧 버튼에 있으면 텍스트 애니메이션 재생
void UTitleUIWidget::OnCreditButtonHovered()
{
	if (CreditTextHover)
	{
		PlayAnimation(CreditTextHover);
	}
}

// 크레딧 버튼에서 벗어나면 텍스트 애니메이션 역방향으로 재생
void UTitleUIWidget::OnCreditButtonUnhovered()
{
	if (CreditTextHover)
	{
		PlayAnimation(CreditTextHover, 0.0f, 1, EUMGSequencePlayMode::Reverse, 1.0f);
	}
}