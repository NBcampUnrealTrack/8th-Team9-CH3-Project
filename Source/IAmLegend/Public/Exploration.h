// Exploration.h
// 요약: 문에 다가가면 오버랩으로 상호작용 가능한 버튼이 생성되어 'F키'를 누르면 StageChoiceWidget으로 넘어감

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Exploration.generated.h"

UCLASS()
class IAMLEGEND_API AExploration : public AActor
{
	GENERATED_BODY()
	
public:	
	AExploration();

protected:
	virtual void BeginPlay() override;

	// F키 입력 처리를 위해 재정의
	virtual void EnableInput(class APlayerController* PlayerController) override;
	virtual void DisableInput(class APlayerController* PlayerController) override;

public:
	// 컴포넌트 선언
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* BoxComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UWidgetComponent* InteractWidgetComp;

	// 띄울 스테이지 UI 위젯 클래스 지정용
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UUserWidget> StageChoiceWidgetClass;

private:
	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// F키 눌렀을 때 실행될 함수
	void OnInteractKeyPressed();

};
