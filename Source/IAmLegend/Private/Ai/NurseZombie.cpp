#include "Ai/NurseZombie.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BrainComponent.h"
#include "Components/SphereComponent.h"
#include "Character/HanPlayerCharacter.h"

ANurseZombie::ANurseZombie()
{
    AttackCooldown = 4.0f;
    Health = 100.0f;
    AttackRange = 200.0f;

    GetCharacterMovement()->MaxWalkSpeed = 150.0f;

    // ✅ 머리 소켓으로 공격 판정 (물어뜯기)
    AttackSphere->SetupAttachment(GetMesh(), FName("Head"));

}

void ANurseZombie::BeginPlay()
{
    Super::BeginPlay();

    UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
    if (AnimInst)
    {
        AnimInst->OnMontageEnded.AddDynamic(this, &ANurseZombie::OnScreamMontageEnded);
    }
}

void ANurseZombie::Tick(float DeltaTime)
{
    if (CurrentState == EZombieState::Dead) return;

    // ✅ Super::Tick은 항상 호출 (이동/물리 처리에 필요)
    Super::Tick(DeltaTime);

    // ✅ Screaming/Hit 상태에선 공격 시도만 막음
    if (CurrentState == EZombieState::Screaming ||
        CurrentState == EZombieState::Hit)
    {
        return;
    }

    // 공격 범위 체크는 Base_Zombie::Tick이 이미 했으므로 추가 처리 불필요
}
void ANurseZombie::PlayAttackMontage()
{
    if (CurrentState == EZombieState::Dead) return;
    if (!AttackMontage || bIsAttacking) return;

    // ✅ 공격 시 플레이어 방향 회전 안 함 (물어뜯기 형태)
    bIsAttacking = true;
    CurrentAttackInstance++;

    GetCharacterMovement()->StopMovementImmediately();
    GetCharacterMovement()->DisableMovement();

    UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
    if (AnimInst)
    {
        AnimInst->Montage_Play(AttackMontage);
    }

    GetWorldTimerManager().SetTimer(AttackTimerHandle, this,
        &ANurseZombie::NurseResetAttack, AttackCooldown, false);
}

void ANurseZombie::NurseResetAttack()
{
    if (CurrentState == EZombieState::Dead) return;

    bIsAttacking = false;
    bHasAppliedDamageThisAttack = false;
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
    GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;

    // ✅ 공격 후 즉시 BT 재개하여 추격 재시작
    AAIController* AIC = Cast<AAIController>(GetController());
    if (AIC && AIC->GetBrainComponent())
    {
        AIC->GetBrainComponent()->ResumeLogic(TEXT("Attack"));
    }
}

void ANurseZombie::PlayScreamMontage()
{
    if (ScreamMontage && CurrentState == EZombieState::Screaming) return;
    if (ScreamMontage && CurrentState == EZombieState::Idle)
    {
        SetCurrentState(EZombieState::Screaming);
        bLostSightDuringScream = false;

        AAIController* AIC = Cast<AAIController>(GetController());
        if (AIC)
        {
            AIC->StopMovement();
            if (AIC->GetBrainComponent())
                AIC->GetBrainComponent()->PauseLogic(TEXT("Screaming"));
        }

        GetCharacterMovement()->MaxWalkSpeed = 0.0f;
        PlayAnimMontage(ScreamMontage);

        GetWorld()->GetTimerManager().SetTimer(ScreamSlowTickHandle, [this]()
            {
                ApplyScreamSlow();
            }, 1.0f, true);
    }
}

void ANurseZombie::ApplyScreamSlow()
{
    if (CurrentState == EZombieState::Dead) return;
    if (!IsValid(this) || !PlayerCharacter) return;

    float Distance = FVector::Dist(GetActorLocation(), PlayerCharacter->GetActorLocation());
    if (Distance > ScreamRadius) return;

    AHanPlayerCharacter* Player = Cast<AHanPlayerCharacter>(PlayerCharacter);
    if (!Player) return;

    Player->GetCharacterMovement()->MaxWalkSpeed = Player->GetBaseWalkSpeed() * 0.5f;
    Player->GetCharacterMovement()->MaxWalkSpeedCrouched =
        (Player->GetBaseWalkSpeed() / 2.0f) * 0.5f;

    GetWorld()->GetTimerManager().ClearTimer(ScreamSlowTimerHandle);
    GetWorld()->GetTimerManager().SetTimer(ScreamSlowTimerHandle, [this]()
        {
            RestorePlayerSpeed();
        }, ScreamSlowDuration, false);
}

void ANurseZombie::RestorePlayerSpeed()
{
    AHanPlayerCharacter* Player = Cast<AHanPlayerCharacter>(PlayerCharacter);
    if (!Player) return;

    Player->GetCharacterMovement()->MaxWalkSpeed = Player->GetBaseWalkSpeed();
    Player->GetCharacterMovement()->MaxWalkSpeedCrouched =
        Player->GetBaseWalkSpeed() / 2.0f;
}

void ANurseZombie::OnScreamMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (Montage != ScreamMontage) return;
    if (CurrentState == EZombieState::Dead) return;
    if (CurrentState != EZombieState::Screaming) return;

    GetWorld()->GetTimerManager().ClearTimer(ScreamSlowTickHandle);

    AAIController* AIC = Cast<AAIController>(GetController());

    if (bLostSightDuringScream)
    {
        if (AIC && AIC->GetBlackboardComponent())
            AIC->GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), nullptr);
        SetCurrentState(EZombieState::Idle);
    }
    else
    {
        // ✅ Scream 종료 후 TargetActor 세팅 → BT가 추격 시작
        if (AIC && AIC->GetBlackboardComponent())
            AIC->GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), PlayerCharacter);
        SetCurrentState(EZombieState::Idle);
    }

    GetWorld()->GetTimerManager().SetTimer(ScreamEndTimerHandle, [this]()
        {
            if (CurrentState == EZombieState::Dead) return;
            GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;
            AAIController* AIC2 = Cast<AAIController>(GetController());
            if (AIC2 && AIC2->GetBrainComponent())
                AIC2->GetBrainComponent()->ResumeLogic(TEXT("Screaming"));
        }, 0.1f, false);
}

float ANurseZombie::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
    class AController* EventInstigator, AActor* DamageCauser)
{
    if (CurrentState == EZombieState::Dead) return 0.0f;

    // ✅ Screaming 중이면 중단만 하고 상태를 Idle로 복구
    // Super 호출 전에 처리해야 Hit 상태로 정상 진입됨
    if (CurrentState == EZombieState::Screaming)
    {
        GetWorld()->GetTimerManager().ClearTimer(ScreamSlowTickHandle);
        StopAnimMontage(ScreamMontage);
        GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;
        SetCurrentState(EZombieState::Idle); // ✅ Idle로 바꿔줘야 Super에서 Hit 처리

        AAIController* AIC = Cast<AAIController>(GetController());
        if (AIC)
        {
            if (AIC->GetBrainComponent())
                AIC->GetBrainComponent()->ResumeLogic(TEXT("Screaming"));
            if (AIC->GetBlackboardComponent())
                AIC->GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), PlayerCharacter);
        }
    }

    // ✅ 나머지는 Base_Zombie와 완전히 동일하게 처리
    return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}
void ANurseZombie::Die()
{
    if (CurrentState == EZombieState::Dead) return;

    CurrentState = EZombieState::Dead;

    if (AAIController* AIC = Cast<AAIController>(GetController()))
    {
        if (AIC->GetBrainComponent())
            AIC->GetBrainComponent()->StopLogic("Dead");
    }

    GetWorldTimerManager().ClearTimer(ScreamSlowTimerHandle);
    GetWorldTimerManager().ClearTimer(ScreamSlowTickHandle);
    GetWorldTimerManager().ClearTimer(ScreamEndTimerHandle);
    GetWorldTimerManager().ClearTimer(HitResumeTimerHandle);
    GetWorldTimerManager().ClearTimer(AttackTimerHandle);
    GetWorldTimerManager().ClearTimer(DamageTimerHandle);
    GetWorldTimerManager().ClearTimer(IdleSoundTimerHandle);
    GetWorldTimerManager().ClearTimer(HitTimerHandle);

    GetCharacterMovement()->StopMovementImmediately();
    GetCharacterMovement()->DisableMovement();

    Super::Die();
}

void ANurseZombie::SetCurrentState(EZombieState NewState)
{
    CurrentState = NewState;

    AAIController* AIC = Cast<AAIController>(GetController());
    if (AIC && AIC->GetBlackboardComponent())
    {
        AIC->GetBlackboardComponent()->SetValueAsEnum(
            TEXT("CurrentState"), (uint8)NewState);
    }
}