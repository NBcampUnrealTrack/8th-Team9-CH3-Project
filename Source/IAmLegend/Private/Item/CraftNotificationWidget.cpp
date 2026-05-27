// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/CraftNotificationWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UCraftNotificationWidget::ShowNotification(const FText& Message, const FLinearColor& TextColor, UUserWidget* InParent)
{
	ParentWidget = InParent;
	
	if (NotificationText)
	{
		NotificationText->SetText(Message);
		NotificationText->SetColorAndOpacity(FSlateColor(TextColor));
	}
}

void UCraftNotificationWidget::NativeConstruct()
{
	Super::NativeConstruct();

	
	if (BackgroundCloseButton)
	{
		BackgroundCloseButton->OnClicked.AddUniqueDynamic(this, &UCraftNotificationWidget::OnBackgroundClicked);
	}
}

void UCraftNotificationWidget::OnBackgroundClicked()
{
	if (ParentWidget)
	{
		ParentWidget->SetKeyboardFocus(); 
	}
	
	RemoveFromParent();
}
