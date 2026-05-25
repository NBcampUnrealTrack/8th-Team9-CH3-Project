#pragma once
#include "CoreMinimal.h"
#include "Ai/Base_Zombie.h"
#include "Character/HanPlayerCharacter.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Boss_PoliceZombie.generated.h"



UCLASS()
class IAMLEGEND_API ABoss_PoliceZombie : public ABase_Zombie
{
    GENERATED_BODY()

public:
    ABoss_PoliceZombie();

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
        class AController* EventInstigator, AActor* DamageCauser) override;
    void PlayExtraActions(bool bDoJumpAttack, bool bDoScream, TFunction<void()> OnComplete);
    void ApplyScreamEffect();

    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimMontage* WalkMontage;
    UPROPERTY(EditAnywhere, Category = "FX")
    class UNiagaraSystem* ScreamEffect;

    UFUNCTION(BlueprintCallable)
    void JumpAttackLand();

    UPROPERTY(EditAnywhere, Category = "Combat")
    float JumpAttackRadius = 2000.0f; // 착지 충격파 범위

    UPROPERTY(EditAnywhere, Category = "Combat")
    float JumpAttackDamage = 30.0f; // 착지 데미지
    UPROPERTY(BlueprintReadWrite, Category = "AI")
    bool bIsScreaming = false;
    // 김민성 작성
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
    float MaxHealth;

    // 코드 추가
    bool bPrologueDone = false;

protected:
    virtual void ResetAttack() override;
    virtual void Tick(float DeltaTime) override;
    virtual void BeginPlay() override;

    // 프롤로그 관련
    // 주석 처리
    // bool bPrologueDone = false;
    FTimerHandle ScreamTimerHandle;

    void StartBiteLoop();
    void OnPrologueTakeDamage();
    void StartCombat();

    // 복사해서 public에 붙었습니다 - 김민성
    //UPROPERTY(EditAnywhere, Category = "Stat")
    //float MaxHealth;

    // 몽타주들
    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimMontage* BiteMontage;

    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimMontage* JumpAttackMontage;

    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimMontage* CriticalHitMontage;

    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimMontage* DownMontage;

    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimMontage* GetUpMontage;

    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimMontage* ScratchMontage;

    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimMontage* ScreamMontage;
    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimMontage* BoundMontage;

    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimMontage* Idle2Montage;

    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimMontage* CrawlMontage;



    // 피격 카운트
    UPROPERTY(EditAnywhere, Category = "Combat")
    int32 BellyHitCount = 0;

    UPROPERTY(EditAnywhere, Category = "Combat")
    int32 BellyHitThreshold = 15;

    UPROPERTY(EditAnywhere, Category = "Combat")
    int32 LegHitCount = 0;

    UPROPERTY(EditAnywhere, Category = "Combat")
    int32 LegHitThreshold = 15;

    bool bIsGroggy = false;
    FTimerHandle GroggyTimerHandle;
    bool bPhaseTransitioned = false;
    bool bCombatStarted = false;
    UPROPERTY(VisibleAnywhere, Category = "Combat")
    class UBoxComponent* BellyHitBox;
    virtual void Die() override;

    UPROPERTY(VisibleAnywhere, Category = "Combat")
    class UBoxComponent* LegHitBox;

    UPROPERTY(VisibleAnywhere, Category = "Combat")
    class USphereComponent* HeartHitSphere;

    UFUNCTION()
    void OnBellyHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnLegHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION(BlueprintCallable)
    void EnableAttack() { EnableAttackCollision(); }

    UFUNCTION(BlueprintCallable)
    void DisableAttack() { DisableAttackCollision(); }

    FTimerHandle ExtraActionTimerHandle;

    // 페이즈 2 관련
    FVector BiteLocation; // Bite하던 위치 저장
    bool bPhase2Triggered = false; // 페이즈 2 한 번만 실행
    FTimerHandle Phase2TimerHandle;

    void StartPhase2();
    void StartCrawlToBiteLocation();
    void OnCrawlArrived();
    void StartPhase2Bite();
    void EndPhase2Bite();
};