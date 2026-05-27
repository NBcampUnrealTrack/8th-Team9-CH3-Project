// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Base_Zombie.generated.h"

// 좀비의 현재 상태를 나타내는 열거형입니다. 블루프린트에서도 볼 수 있습니다.

UENUM(BlueprintType)
enum class EZombieState : uint8
{
    Idle,
    Screaming,    // ← 추가
    Attacking,
    Hit,
    Knockdown,    // ← 추가
    Dead
};

UCLASS()
class IAMLEGEND_API ABase_Zombie : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABase_Zombie();
    // Called every frame
    virtual void Tick(float DeltaTime) override;
    // 언리얼 내장 데미지 시스템: 누군가 이 좀비를 때리면 이 함수가 자동으로 실행됩니다.
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
    UFUNCTION(BlueprintCallable, Category = "ZombieState")
    EZombieState GetCurrentState() const { return CurrentState; }

    UFUNCTION(BlueprintCallable, Category = "ZombieState")
    virtual void SetCurrentState(EZombieState NewState) { CurrentState = NewState; }

    UFUNCTION(BlueprintCallable)
    virtual void PlayAttackMontage();
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* GetUpMontage;

    bool bIsAttacking = false; // 공격 중일 때 중복 공격을 막기 위한 변수입니다.
    // 공격 사거리 변수화 (좀비마다 다를 수 있음)
    UPROPERTY(EditAnywhere, Category = "Combat")
    float AttackRange;

    // [에디터 설정] 좀비의 체력
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
    float Health;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, Category = "Animation")
    float DeathAnimLength = 3.0f;
    // [에디터 설정] 각 상황에 맞는 애니메이션 몽타주 에셋을 넣어주세요.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    class UAnimMontage* AttackMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    class UAnimMontage* HitMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    class UAnimMontage* DeathMontage;

    // AI가 추적 중인 플레이어 캐릭터를 담아둘 변수입니다.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    class ACharacter* PlayerCharacter;
    // 대기 소리 컴포넌트입니다.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
    class UAudioComponent* IdleSoundComponent;

    // 사망 시 재생할 사운드 에셋
    UPROPERTY(EditAnywhere, Category = "Audio")
    class USoundBase* DeathSound;


    
    // [에디터 설정] 공격 속도(간격)를 조절합니다. 4.0이면 4초마다 공격합니다.
    UPROPERTY(EditAnywhere, Category = "Combat")
    float AttackCooldown;

    float DefaultMaxWalkSpeed;

    // 시간 측정을 위한 타이머 핸들입니다.
    FTimerHandle AttackTimerHandle;

    // 데미지 처리를 위한 타이머 핸들입니다.
    FTimerHandle DamageTimerHandle;

    // 소리 재개 타이머 핸들
    FTimerHandle IdleSoundTimerHandle;
    EZombieState CurrentState = EZombieState::Idle;
    FTimerHandle DeathTimerHandle;
    FTimerHandle HideTimerHandle;
    FTimerHandle DeathFreezeTimerHandle;
    FTimerHandle HitTimerHandle;

    // [에디터 설정] 좀비의 체력입니다.
    // 복사해서 public로 복제했습니다 - 김민성
    //UPROPERTY(EditAnywhere, Category = "Stat")
    //float Health;


    // 공격 후 쿨타임이 지나면 다시 공격 가능하게 해주는 함수입니다.
    virtual void ResetAttack();

    // 체력이 0이 되었을 때의 처리를 담당합니다.
    virtual void Die();
    UFUNCTION()
    virtual void OnAttackOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    UFUNCTION(BlueprintCallable)
    virtual void EnableAttackCollision();
    UFUNCTION(BlueprintCallable)
    virtual void DisableAttackCollision();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* KnockdownMontage;

    UFUNCTION(BlueprintCallable)
    void PlayKnockdownMontage();
    // 소리 재개 함수
    UFUNCTION()
    void ResumeIdleSound();

    // 공격 판정을 위한 구체 충돌체
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zombie | Combat")
    class USphereComponent* AttackSphere;

    // 플레이어에게 줄 데미지 양
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie | Combat")
    float AttackDamage = 10.f;

    bool bHasAppliedDamageThisAttack = false; // 이번 휘두름에 때렸는지 체크
    int32 CurrentAttackInstance = 0; // 이번 공격이 몇 번째 휘두름인지 기록
    int32 LastAppliedDamageInstance = -1; // 마지막으로 데미지를 준 휘두름 번호

    // 마지막으로 데미지를 입힌 시점의 게임 시간을 저장
    float LastDamageTime = 0.0f;

    // 잠금 시간 (예: 2초 동안은 추가 데미지 무시)
    const float DamageLockDuration = 0.3f;
};
