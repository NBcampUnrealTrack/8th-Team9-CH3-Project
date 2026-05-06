#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MainGameInstance.generated.h"


UCLASS()
class IAMLEGEND_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UMainGameInstance();
	
	//게임 시작 여부 설정 및 확인
	void SetGameStarted(bool bStarted);
	bool GetGameStarted() const;
	
	//UI 팝업 여부 설정 및 확인 / 마우스 커서 필요한가 아닌가
	void SetUIPopUp(bool bUIPopUp);
	bool GetUIPopUp() const;
private:
	
	bool bIsGameStarted;
	bool bIsUIPopUp;
};
