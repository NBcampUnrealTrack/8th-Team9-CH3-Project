// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ai/Base_Zombie.h"
#include "NurseZombie.generated.h"

/**
 * 
 */
UCLASS()
class IAMLEGEND_API ANurseZombie : public ABase_Zombie
{
	GENERATED_BODY()
	
public:
    ANurseZombie();
    virtual void BeginPlay() override;
   
    // 1. 매 프레임 실행되는 함수 (부모의 기능을 선택적으로 쓰기 위해 오버라이드)
    virtual void Tick(float DeltaTime) override;

    // 2. 비명 애니메이션 실행 (뇌(AI)의 명령을 받아 몸이 움직이는 함수)
    UFUNCTION(BlueprintCallable, Category = "Animation")
    virtual void PlayScreamMontage();

    // 3. 상태 변경 (내부 상태와 AI 블랙보드를 동기화하는 핵심 함수)
    virtual void SetCurrentState(EZombieState NewState) override;

    // 4. 데미지 시스템 (피격 시 비명을 멈추고 추격으로 전환하는 로직)
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    class UAnimMontage* ScreamMontage;

    UFUNCTION()
    void OnScreamMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    // 시야 상실 여부를 추적할 플래그
    bool bLostSightDuringScream = false;
};