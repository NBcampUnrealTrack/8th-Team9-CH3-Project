// CreditUIWidget.cpp


#include "UI/CreditUIWidget.h"
#include "Components/Button.h"

void UCreditUIWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (CloseButton)
	{
		// 버튼 클릭 시 OnCloseClicked 함수 연결
		CloseButton->OnClicked.RemoveDynamic(this, &UCreditUIWidget::OnCloseClicked);
		CloseButton->OnClicked.AddDynamic(this, &UCreditUIWidget::OnCloseClicked);
	}
}

void UCreditUIWidget::OnCloseClicked()
{
	// 화면에서 위젯 제거
	RemoveFromParent();
}
