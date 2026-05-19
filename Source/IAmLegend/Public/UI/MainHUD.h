#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainHUD.generated.h"

class UPauseMenuWidget; // 전방 선언 (일시정지 메뉴)
class UPlayerHealthWidget; // 전방 선언(체력바)
class UCrosshairWidget; // 전방 선언(조준점)
class UWeaponInstallationWidget; // 전방 선언(무기 상태)
class UWeaponSlotWidget; // 전방 선언(무기 슬롯)

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
	UPlayerHealthWidget* GetPlayerHealthWidget() const { return PlayerHealthWidget; }

	//탈출 지점 UI 관리
	void ShowExtractionHUD();
	void HideExtractionHUD();
	
	//탈출 결과 UI 관리 
	void ShowStageResultHUD();
	void HideStageResultHUD();

	// 크레딧 UI 관리
	void ShowCreditHUD();

	// 옵션 UI 관리
	void ShowOptionHUD();

	// GameOver UI 관리
	void ShowGameOverHUD();

	// Crosshair UI 관리
	void ShowCrosshairHUD();
	void HideCrosshairHUD();

	// 무기 UI 관리
	void ShowWeaponHUD();
	void HideWeaponHUD();

	// 무기 슬롯 UI 관리
	void ShowWeaponSlotHUD();
	void HideWeaponSlotHUD();

protected:
	//사용할 Widget 클래스 설정
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> TitleHUDClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> StageHUDClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UPauseMenuWidget> PauseMenuClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UPlayerHealthWidget> PlayerHealthClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> ExtractionHUDClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> CreditHUDClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> OptionHUDClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> GameOverClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> StageResultHUDClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UCrosshairWidget> CrosshairClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UWeaponInstallationWidget> WeaponHUDClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UWeaponSlotWidget> WeaponSlotClass;
	
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
	UPROPERTY()
	UUserWidget* StageResultHUDWidget;
	UPROPERTY()
	UCrosshairWidget* CrosshairWidget;
	UPROPERTY()
	UWeaponInstallationWidget* WeaponHUDWidget;
	UPROPERTY()
	UWeaponSlotWidget* WeaponSlotWidget;
};
