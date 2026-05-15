// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/StageHUDWidget.h"

#include "Components/TextBlock.h"
#include "Gamemode/MainGameModeBase.h"
#include "Kismet/GameplayStatics.h"


void UStageHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	GetWorld()->GetTimerManager().SetTimer(UpdateTimerHandle, this, &UStageHUDWidget::UpdateStageHUD, 0.1f, true);
	
	UpdateKillCount();
	
}

void UStageHUDWidget::UpdateStageHUD()
{
	AMainGameModeBase* GM = Cast<AMainGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	
	if (GM)
	{
		CurrentRemainingTime = GM->GetRemainingStageTime();
	}
	
	if (Text_RemainingTime)
	{
		Text_RemainingTime->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), FMath::Max(CurrentRemainingTime, 0.0f))));
	}
}
/*
void UStageHUDWidget::UpdateRemainingTime(float RemainingSeconds)
{
	CurrentRemainingTime = RemainingSeconds;
	
	if (Text_RemainingTime)
	{
		Text_RemainingTime->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), FMath::Max(CurrentRemainingTime, 0.0f))));
	}
}
*/
void UStageHUDWidget::UpdateKillCount()
{
	AMainGameModeBase* GM = Cast<AMainGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	
	if (GM)
	{
		Text_KillCount->SetText(FText::FromString(FString::FromInt(GM->GetPlayerKillCount())));
	}
}