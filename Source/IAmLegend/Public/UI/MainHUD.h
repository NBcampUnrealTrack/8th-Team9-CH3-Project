#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainHUD.generated.h"


UCLASS()
class IAMLEGEND_API AMainHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	
	//타이틀 UI 관리
	void ShowTitleHUD();
	void HideTitleHUD();
	//스테이지 UI 관리
	void ShowStageHUD();
	void HideStageHUD();
	//탈출 타이머 UI 관리
	void ShowEscapeTimerHUD();
	void HideEscapeTimerHUD();
	
protected:
	//사용할 Widget 클래스 설정
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> TitleHUDClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> StageHUDClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> EscapeTimerHUDClass;
	
private:
	
	//인스턴스
	UPROPERTY()
	UUserWidget* TitleHUDWidget;
	UPROPERTY()
	UUserWidget* StageHUDWidget;
	UPROPERTY()
	UUserWidget* EscapeTimerHUDWidget;
};
