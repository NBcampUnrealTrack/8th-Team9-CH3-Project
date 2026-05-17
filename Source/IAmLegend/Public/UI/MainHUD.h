#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainHUD.generated.h"

class UPauseMenuWidget; // 전방 선언 (일시정지 메뉴)
class UPlayerHealthWidget; // 전방 선언(체력바)

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
	// 일시정지 메뉴 UI 관리
	UFUNCTION(BlueprintCallable, Category = "UI")
	void TogglePauseMenu();
	// 체력 UI 관리
	void ShowPlayerHealthHUD();
	void HidePlayerHealthHUD();
	 
	// BP_캐릭터에 접근하기 위한 Getter
	UFUNCTION(BlueprintCallable, Category = "UI")
	//UPlayerHealthWidget* GetPlayerHealthWidget() const { return PlayerHealthWidget; }

	//탈출 지점 UI 관리
	void ShowExtractionHUD();
	void HideExtractionHUD();

	// 크레딧 UI 관리
	void ShowCreditHUD();

	// 옵션 UI 관리
	void ShowOptionHUD();

	// GameOver UI 관리
	void ShowGameOverHUD();
	
protected:
	//사용할 Widget 클래스 설정
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> TitleHUDClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> StageHUDClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UPauseMenuWidget> PauseMenuClass;
	//UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	//TSubclassOf<UPlayerHealthWidget> PlayerHealthClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> ExtractionHUDClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> CreditHUDClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> OptionHUDClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> GameOverClass;
	
private:
	
	//인스턴스
	UPROPERTY()
	UUserWidget* TitleHUDWidget;
	UPROPERTY()
	UUserWidget* StageHUDWidget;
	UPROPERTY()
	UPauseMenuWidget* PauseMenuWidget;
	UPROPERTY()
	UPlayerHealthWidget* PlayerHealthWidget;
	UPROPERTY()
	UUserWidget* ExtractionHUDWidget;
};
