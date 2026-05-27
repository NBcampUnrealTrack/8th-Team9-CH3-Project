#include "UI/StageResultHUDWidget.h"
#include "GameMode/MainGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Gamemode/MainGameStateBase.h"
#include "UI/ItemResultEntryWidget.h"
#include "Components/HorizontalBox.h"

void UStageResultHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (Btn_Return)
	{
		Btn_Return->OnClicked.AddDynamic(this, &UStageResultHUDWidget::OnReturnButtonClicked);
	}
	
	//스테이지 남은 시간 초기화
	UpdateStageRemainingTime();
	//죽인 좀비 수 초기화
	UpdateKillCount();
	//아이템 목록 출력
	PopulateAcquiredItems();
}

void UStageResultHUDWidget::OnReturnButtonClicked()
{
	UGameplayStatics::SetGamePaused(GetWorld(), false);
	
	AMainGameModeBase* GM = Cast<AMainGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (!GM) return;
	GM->LoadStageLevel(EStageType::Shelter);
}

void UStageResultHUDWidget::UpdateStageRemainingTime()
{
	AMainGameModeBase* GM = Cast<AMainGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	
	if (GM && Text_StageRemainingTime)
	{
		float CurrentRemainingTime = GM->GetRemainingStageTime();
		
		float ClampedTime = FMath::Max(CurrentRemainingTime, 0.0f);
		int32 Minutes = FMath::FloorToInt(ClampedTime/60.0f);
		int32 Seconds = FMath::FloorToInt(FMath::Fmod(ClampedTime, 60.0f));
		
		FString String = FString::Printf(TEXT("%2d:%2d"), Minutes, Seconds);
		Text_StageRemainingTime->SetText(FText::FromString(String));
	}
}

void UStageResultHUDWidget::UpdateKillCount()
{
	
	AMainGameStateBase* GS = Cast<AMainGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
	if (GS && Text_KillCount)
	{
		FString String = FString::Printf(TEXT("%d"), GS->GetPlayerKillCount());
		Text_KillCount->SetText(FText::FromString(String));
	}
	
}

void UStageResultHUDWidget::PopulateAcquiredItems()
{
	if (!HB_AcquiredItems || !ItemEntryWidgetClass) return;
	
	//기존 항목 초기화
	HB_AcquiredItems->ClearChildren();
	
	AMainGameStateBase* GS = Cast<AMainGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
	if (!GS) return;
	
	for (const FItemSlot& ItemSlot : GS->GetAcquiredItems())
	{
		if (!ItemSlot.ItemData) continue;
		
		UItemResultEntryWidget* ItemEntryWidget = CreateWidget<UItemResultEntryWidget>(GetWorld(), ItemEntryWidgetClass);
		
		if (ItemEntryWidget)
		{
			ItemEntryWidget->InitEntry(ItemSlot.ItemData, ItemSlot.Quantity);
			HB_AcquiredItems->AddChild(ItemEntryWidget);
		}
	}
}