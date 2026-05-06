// Fill out your copyright notice in the Description page of Project Settings.


#include "TestUserWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UTestUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (Btn_GameStart)
	{
		Btn_GameStart->OnClicked.AddDynamic(this, &UTestUserWidget::OnGameStartClicked);
	}

	if (Txt_Shelter)
	{
		Txt_Shelter->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UTestUserWidget::OnGameStartClicked()
{
	Btn_GameStart->SetVisibility(ESlateVisibility::Hidden);
	Txt_Shelter->SetVisibility(ESlateVisibility::Visible);
}