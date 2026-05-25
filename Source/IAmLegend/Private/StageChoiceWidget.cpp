// StageChoiceWidget.cpp
// 요약: 스테이지 선택 UI 화면 출력

#include "StageChoiceWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Kismet/GameplayStatics.h"
#include "Gamemode/MainGameModeBase.h"
#include "Gamemode/MainGameStateBase.h"
#include "Gamemode/MainGameInstance.h"
#include "UI/MainHUD.h"
#include "Character/HanPlayerCharacter.h"
#include "Item/InventoryComponent.h"
#include "Item/ItemDataAsset.h"

void UStageChoiceWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Hospital) Hospital->OnClicked.AddDynamic(this, &UStageChoiceWidget::OnHospitalClicked);
	if (School) School->OnClicked.AddDynamic(this, &UStageChoiceWidget::OnSchoolClicked);
	if (Police) Police->OnClicked.AddDynamic(this, &UStageChoiceWidget::OnPoliceClicked);
	if (Back) Back->OnClicked.AddDynamic(this, &UStageChoiceWidget::OnBackClicked);
	if (Boss) Boss->OnClicked.AddDynamic(this, &UStageChoiceWidget::OnBossClicked);

	// 페이드 아웃 타이머 초기값
	FadeDuration = 2.0f;
	FadeElapsed = 0.0f;
}

void UStageChoiceWidget::OnHospitalClicked()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(false);
	}
	AMainGameModeBase* GameMode = Cast<AMainGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode) 
	{
		GameMode->EnterStage(EStageType::Hospital);
	}
	
}

void UStageChoiceWidget::OnSchoolClicked()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(false);
	}

	AMainGameModeBase* GameMode = Cast<AMainGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode) 
	{
		GameMode->EnterStage(EStageType::School);
	}
}

void UStageChoiceWidget::OnPoliceClicked()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(false);
	}

	AMainGameModeBase* GameMode = Cast<AMainGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode) 
	{
		GameMode->EnterStage(EStageType::PoliceStation);
	}
}

void UStageChoiceWidget::OnBackClicked()
{
	RemoveFromParent();

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(false);
	}
}

void UStageChoiceWidget::OnBossClicked()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return;

	// GameInstance 불러와서 백신 아이템 확인
	UMainGameInstance* GI = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (!GI) return;

	bool bPossessVaccine = false;

	// 백신 확인
	for (const FItemSlot& ItemSlot : GI->GetItemsFromGlobalInventory())
	{
		if (ItemSlot.ItemData && ItemSlot.ItemData->ItemName.Equals(TEXT("백신")))
		{
			bPossessVaccine = true;
			break;
		}
	}

	// 경고 UI 출력 함수
	if (bPossessVaccine)
	{
		// 백신을 보유 있으면 보스 스테이지 이동
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(false);

		AMainGameModeBase* GameMode = Cast<AMainGameModeBase>(UGameplayStatics::GetGameMode(this));
		if (GameMode)
		{
			GameMode->EnterStage(EStageType::Boss);
		}
	}
	else
	{
		// 백신 없으면 경고 UI 함수 호출
		PlayWarningSequence();
	}
}

void UStageChoiceWidget::PlayWarningSequence()
{
	if (WarningBorder)
	{
		// 기존 타이머가 돌고 있다면 초기화
		GetWorld()->GetTimerManager().ClearTimer(FadeTimerHandle);

		// 텍스트를 화면에 보이게 켜고 완전 불투명화 처리
		WarningBorder->SetVisibility(ESlateVisibility::Visible);
		WarningBorder->SetRenderOpacity(1.0f);

		FadeElapsed = 0.0f;

		// 페이드아웃 타이머 가동
		GetWorld()->GetTimerManager().SetTimer(
			FadeTimerHandle,
			this,
			&UStageChoiceWidget::UpdateFade,
			0.016f,
			true
		);
	}
}

// 틱당 투명도 변화 연출 함수
void UStageChoiceWidget::UpdateFade()
{
	FadeElapsed += 0.016f;

	float Alpha = FMath::Clamp(1.0f - (FadeElapsed / FadeDuration), 0.0f, 1.0f);

	if (WarningBorder)
	{
		WarningBorder->SetRenderOpacity(Alpha);
	}

	if (FadeElapsed >= FadeDuration)
	{
		GetWorld()->GetTimerManager().ClearTimer(FadeTimerHandle);
		if (WarningBorder)
		{
			WarningBorder->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}