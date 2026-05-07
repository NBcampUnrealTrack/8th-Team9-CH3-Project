#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

UINTERFACE(MinimalAPI)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class IAMLEGEND_API IInteractableInterface
{
	GENERATED_BODY()

public:
	//상호작용 실행 함수
	virtual void Interact(ACharacter* Player) = 0;
	
	//UI에 표시할 텍스트 (아이템 줍기, 문 열기 등등 편하신대로 사용하십시옹)
	virtual FString InteractionText() const = 0;
	
	// 플레이어가 범위에 들어옴 (UI 켜기)
	virtual void OnPlayerEntered(ACharacter* Player) = 0;

	// 플레이어가 범위를 벗어남 (UI 끄기)
	virtual void OnPlayerExited(ACharacter* Player) = 0;
};
