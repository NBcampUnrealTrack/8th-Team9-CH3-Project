// PlayerHealthWidget.h
// 요약 : 캐릭터가 데미지를 받으면 체력 감소

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHealthWidget.generated.h"

// 전방 선언
class UProgressBar;
class UTextBlock;

UCLASS()
class IAMLEGEND_API UPlayerHealthWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	// meta = (BindWidget)는 블루프린트 위젯의 부품과 C++ 코드가 자동으로 연결되는 문법
	UPROPERTY(meta = (BindWidget))
	UProgressBar* pbHP;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* txtHP;

public:
	// 캐릭터가 데미지를 입을 때 체력을 갱신해 줄 함수
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateHP(float CurrentHP, float MaxHP);
};
