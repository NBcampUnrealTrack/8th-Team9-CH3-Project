#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StageResultHUDWidget.generated.h"

UCLASS()
class IAMLEGEND_API UStageResultHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	// 스테이지 남은 시간 표시 텍스트
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_StageRemainingTime;

	// 죽인 좀비 수 표시 텍스트
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_KillCount;

	// 쉘터 복귀 버튼
	UPROPERTY(meta = (BindWidget))
	class UButton* Btn_Return;

private:
	// 쉘터 복귀 버튼 클릭 이벤트
	UFUNCTION()
	void OnReturnButtonClicked();
	// 스테이지 남은 시간 업데이트
	void UpdateStageRemainingTime();
	// 죽은 좀비 수 업데이트
	void UpdateKillCount();
	

};
