#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StageHUDWidget.generated.h"


UCLASS()
class IAMLEGEND_API UStageHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_RemainingTime;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_KillCount;
	
private:
	float CurrentRemainingTime;
	
	//스테이지 UI 업데이트
	void UpdateStageHUD();
	//스테이지 제한 시간 업데이트
	void UpdateRemainingTime(float RemainingSeconds);
	//플레이어 킬 정보 업데이트
	void UpdateKillCount();
	FTimerHandle UpdateTimerHandle;
};
