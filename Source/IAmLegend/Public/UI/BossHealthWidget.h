// BossHealthWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BossHealthWidget.generated.h"

UCLASS()
class IAMLEGEND_API UBossHealthWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* BossProgressBar;

protected:
	// 위젯이 생성될 때 실행
	virtual void NativeConstruct() override;

	// 위젯이 매 프레임 스스로 로직을 체크하는 함수
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	const float BossDetectDistance = 1000.f;
};
