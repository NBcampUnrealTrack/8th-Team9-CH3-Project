// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AIPerceptionComponent.h" // 이 줄이 빠져서 에러가 납니다!
#include "Perception/AISenseConfig_Sight.h"    // 시야 설정을 위해 이것도 필요합니다.
#include "PoliceZombie_ai.generated.h"

class UBlackboardComponent;
class UBehaviorTreeComponent;

/**
 * 
 */
UCLASS()
class IAMLEGEND_API APoliceZombie_ai : public AAIController
{
	GENERATED_BODY()
	
public:
	APoliceZombie_ai(); // 생성자: AI가 태어날 때 기본 설정(시야 등)을 세팅합니다.

protected:
	virtual void OnPossess(APawn* InPawn) override; // 이 AI 컨트롤러가 특정 좀비(Pawn)를 조종하기 시작할 때 호출되는 함수입니다.

public:
	// AI의 오감 중 '시각'을 담당하는 컴포넌트입니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class UAIPerceptionComponent* AIPerceptionComp;

	// 시야의 거리, 각도 등을 세밀하게 설정하는 데이터 객체입니다.
	class UAISenseConfig_Sight* SightConfig;

	// 무언가(플레이어 등)를 감지했을 때 '아! 봤다!'라고 반응할 함수입니다.
	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus Stimulus);

	// AI의 기억장소(Blackboard)를 관리하는 컴포넌트입니다.
	UPROPERTY()
	UBlackboardComponent* BlackboardComp;

	// AI의 행동 패턴(Behavior Tree)을 제어하는 컴포넌트입니다.
	UPROPERTY()
	UBehaviorTreeComponent* BehaviorComp;

	// [에디터 설정] 사용할 비헤이비어 트리 에셋을 등록합니다. (블루프린트 디테일 창에서 선택)
	UPROPERTY(EditAnywhere, Category = "AI")
	class UBehaviorTree* BTAsset;

	// [에디터 설정] 사용할 블랙보드 데이터 에셋을 등록합니다.
	UPROPERTY(EditAnywhere, Category = "AI")
	class UBlackboardData* BBAsset;


};
