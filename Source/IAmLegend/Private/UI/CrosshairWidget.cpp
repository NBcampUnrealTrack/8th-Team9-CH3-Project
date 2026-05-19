// CoreMinimal.cpp

#include "UI/CrosshairWidget.h"
#include "Components/Image.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/HanPlayerCharacter.h"
#include "TimerManager.h"

void UCrosshairWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 게임 시작 시 히트마커는 보이지 않게 숨김 처리
	if (HitMarker)
	{
		HitMarker->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UCrosshairWidget::ShowHitMarker()
{
	if (HitMarker)
	{
		// 히트마커 표시
		HitMarker->SetVisibility(ESlateVisibility::HitTestInvisible);

		// 이전에 작동 중인 타이머가 있다면 초기화 (연속으로 맞출 경우 타이머 갱신)
		GetWorld()->GetTimerManager().ClearTimer(HitMarkerTimerHandle);

		// 0.2초 후에 HideHitMarker 함수를 실행하여 마커를 숨김 (원하는 시간으로 조절 가능)
		GetWorld()->GetTimerManager().SetTimer(HitMarkerTimerHandle, this, &UCrosshairWidget::HideHitMarker, 0.2f, false);
	}
}

void UCrosshairWidget::HideHitMarker()
{
	if (HitMarker)
	{
		HitMarker->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UCrosshairWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	AHanPlayerCharacter* PlayerCharacter = Cast<AHanPlayerCharacter>(GetOwningPlayerPawn());
	if (PlayerCharacter)
	{
		// 상태에 따른 TargetSpread 계산
		float Speed = PlayerCharacter->GetVelocity().Size2D();
		bool bIsFalling = PlayerCharacter->GetCharacterMovement()->IsFalling();
		bool bIsAiming = PlayerCharacter->IsAiming();

		// 조준 중일 때만 조준선(Line)이 보이게 설정
		ESlateVisibility LineVisibility = bIsAiming ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden;

		if (TopLine) TopLine->SetVisibility(LineVisibility);
		if (BottomLine) BottomLine->SetVisibility(LineVisibility);
		if (LeftLine) LeftLine->SetVisibility(LineVisibility);
		if (RightLine) RightLine->SetVisibility(LineVisibility);

		// 벌어짐(Spread) 계산
		BaseSpread = 15.0f;
		TargetSpread = BaseSpread;

		if (Speed > 0.f)
		{
			TargetSpread += (Speed / 10.0f);
		}
		if (bIsFalling)
		{
			TargetSpread += 40.0f;
		}
		if (bIsAiming)
		{
			// 조준 중일 때 조준점과 최소 간격 유지
			TargetSpread = 20.0f;
		}

		// Crosshair 움직임을 부드럽게 보간
		CurrentSpread = FMath::FInterpTo(CurrentSpread, TargetSpread, InDeltaTime, 15.0f);

		// 조준선 업데이트
		if (TopLine) TopLine->SetRenderTranslation(FVector2D(0.f, -CurrentSpread));
		if (BottomLine) BottomLine->SetRenderTranslation(FVector2D(0.f, CurrentSpread));
		if (LeftLine) LeftLine->SetRenderTranslation(FVector2D(-CurrentSpread, 0.f));
		if (RightLine) RightLine->SetRenderTranslation(FVector2D(CurrentSpread, 0.f));
	}
}