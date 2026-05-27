#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StageHUDWidget.generated.h"

class AMainGameModeBase;

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
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Text_CurrentStage;
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_SkullIcon;
	
private:
	float CurrentRemainingTime;
	
	//스테이지 UI 업데이트
	void UpdateStageHUD();
	//스테이지 제한 시간 업데이트
	void UpdateRemainingTime(float RemainingSeconds);
	//플레이어 킬 정보 업데이트
	void UpdateKillCount();
	//시간 포멧 변환
	FString ConvertToClockTime(float RemainingSeconds) const;
	//점멸 이펙트 관리
	void StartBlinkEffect();
	void StopBlinkEffect();
	void OnBlink();
	
	//GameMode 캐싱
	UPROPERTY()
	AMainGameModeBase* CachedGM;
	
	FTimerHandle UpdateTimerHandle;
	//점멸용 타이머
	FTimerHandle BlinkTimerHandle;
	//점멸 시간
	float BlinkTime;
	
	//점멸 관련 변수
	bool bIsBlinkVisible;
	bool bAlphaGoingDown;
	float CurrentAlpha;
};
