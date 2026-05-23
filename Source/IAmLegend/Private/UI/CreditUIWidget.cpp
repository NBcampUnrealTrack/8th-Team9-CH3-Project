// CreditUIWidget.cpp


#include "UI/CreditUIWidget.h"
#include "Components/Button.h"

void UCreditUIWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 키 입력 포커스 설정
	bIsFocusable = true;
	SetKeyboardFocus();

	if (CloseButton)
	{
		// 버튼 클릭 시 OnCloseClicked 함수 연결
		CloseButton->OnClicked.RemoveDynamic(this, &UCreditUIWidget::OnCloseClicked);
		CloseButton->OnClicked.AddDynamic(this, &UCreditUIWidget::OnCloseClicked);
	}
}

// esc 입력 시
FReply UCreditUIWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		OnCloseClicked(); // 닫기 버튼을 누른 것과 동일하게 처리
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UCreditUIWidget::OnCloseClicked()
{
	// 화면에서 위젯 제거
	RemoveFromParent();
}
