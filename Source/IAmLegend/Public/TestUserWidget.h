#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TestUserWidget.generated.h"

class UButton;
class UTextBlock;

UCLASS()
class IAMLEGEND_API UTestUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_GameStart;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Txt_Shelter;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UButton> Btn_SelectSchoolStage;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UButton> Btn_SelectPoliceStage;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UButton> Btn_SelectHospitalStage;
	
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnGameStartClicked();
	UFUNCTION()
	void OnSchoolStageClicked();
	UFUNCTION()
	void OnHospitalStageClicked();
	UFUNCTION()
	void OnOnPoliceStageClicked();
};
