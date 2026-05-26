// StageChoiceWidget.cpp
// 요약: 스테이지 선택 UI 화면 출력

#include "StageChoiceWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/Image.h"
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

	if (Hospital)
	{
		Hospital->OnClicked.AddDynamic(this, &UStageChoiceWidget::OnHospitalClicked);
		Hospital->OnHovered.AddDynamic(this, &UStageChoiceWidget::OnHospitalHovered);
		Hospital->OnUnhovered.AddDynamic(this, &UStageChoiceWidget::HideInfoPanel);
	}
	if (School)
	{
		School->OnClicked.AddDynamic(this, &UStageChoiceWidget::OnSchoolClicked);
		School->OnHovered.AddDynamic(this, &UStageChoiceWidget::OnSchoolHovered);
		School->OnUnhovered.AddDynamic(this, &UStageChoiceWidget::HideInfoPanel);
	}
	if (Police)
	{
		Police->OnClicked.AddDynamic(this, &UStageChoiceWidget::OnPoliceClicked);
		Police->OnHovered.AddDynamic(this, &UStageChoiceWidget::OnPoliceHovered);
		Police->OnUnhovered.AddDynamic(this, &UStageChoiceWidget::HideInfoPanel);
	}
	if (Boss)
	{
		Boss->OnClicked.AddDynamic(this, &UStageChoiceWidget::OnBossClicked);
		Boss->OnHovered.AddDynamic(this, &UStageChoiceWidget::OnBossHovered);
		Boss->OnUnhovered.AddDynamic(this, &UStageChoiceWidget::HideInfoPanel);
	}
	if (Back)
	{
		Back->OnClicked.AddDynamic(this, &UStageChoiceWidget::OnBackClicked);
	}

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

void UStageChoiceWidget::OnHospitalHovered()
{
	if (InfoTitleText && InfoDescText && InfoPanel && InfoImage)
	{
		InfoTitleText->SetText(FText::FromString(TEXT("병원 (Hospital)")));
		InfoDescText->SetText(FText::FromString(TEXT("의료품과 의약품이 남아있을 가능성이 높은 병원입니다.\n\n많은 좀비들이 배회하고 있습니다.")));

		if (HospitalTex) InfoImage->SetBrushFromTexture(HospitalTex); // 이미지 변경

		InfoPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UStageChoiceWidget::OnSchoolHovered()
{
	if (InfoTitleText && InfoDescText && InfoPanel && InfoImage)
	{
		InfoTitleText->SetText(FText::FromString(TEXT("학교 (School)")));
		InfoDescText->SetText(FText::FromString(TEXT("과거 대피소로 사용되었던 학교입니다.\n\n생존에 필요한 물자를 탐색할 수 있습니다.")));

		if (SchoolTex) InfoImage->SetBrushFromTexture(SchoolTex); // 이미지 변경

		InfoPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UStageChoiceWidget::OnPoliceHovered()
{
	if (InfoTitleText && InfoDescText && InfoPanel && InfoImage)
	{
		InfoTitleText->SetText(FText::FromString(TEXT("경찰서 (Police)")));
		InfoDescText->SetText(FText::FromString(TEXT("무기와 탄약을 확보할 수 있는 경찰서입니다.\n\n좀비들이 몰려 있어 위험합니다.")));

		if (PoliceTex) InfoImage->SetBrushFromTexture(PoliceTex); // 이미지 변경

		InfoPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UStageChoiceWidget::OnBossHovered()
{
	if (InfoTitleText && InfoDescText && InfoPanel && InfoImage)
	{
		InfoTitleText->SetText(FText::FromString(TEXT("검문소 (Checkpoint)")));
		InfoDescText->SetText(FText::FromString(TEXT("이곳에서 빠져나갈 수 있는 유일한 탈출구입니다.\n\n[입장 조건: 백신]")));

		if (BossTex) InfoImage->SetBrushFromTexture(BossTex); // 이미지 변경

		InfoPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UStageChoiceWidget::HideInfoPanel()
{
	if (InfoPanel)
	{
		InfoPanel->SetVisibility(ESlateVisibility::Hidden);
	}
}