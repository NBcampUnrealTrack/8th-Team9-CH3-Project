#include "Ai/NurseZombie.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BrainComponent.h"
#include "Character/HanPlayerCharacter.h"

ANurseZombie::ANurseZombie()
{
    AttackCooldown = 4.0f;
    Health = 100.0f;
    AttackRange = 200.0f;

    GetCharacterMovement()->MaxWalkSpeed = 150.0f;
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

    Super::Tick(DeltaTime);

    if (CurrentState == EZombieState::Screaming ||
        CurrentState == EZombieState::Dead)
    {
        return;
    }
}

void ANurseZombie::PlayScreamMontage()
{
    if (ScreamMontage && CurrentState == EZombieState::Screaming) return; // 중복 방지
    if (ScreamMontage && CurrentState == EZombieState::Idle)
    {
        SetCurrentState(EZombieState::Screaming);
        bLostSightDuringScream = false;

        AAIController* AIC = Cast<AAIController>(GetController());
        if (AIC)
        {
            AIC->StopMovement();
            AIC->GetBrainComponent()->PauseLogic(TEXT("Screaming")); // BT 일시정지
        }

        // 이동속도 0으로 강제 차단
        GetCharacterMovement()->MaxWalkSpeed = 0.0f;

        PlayAnimMontage(ScreamMontage);

        // 틱 타이머 시작
        GetWorld()->GetTimerManager().SetTimer(ScreamSlowTickHandle, [this]()
            {
                ApplyScreamSlow();
            }, 1.0f, true);
    }
}

void ANurseZombie::ApplyScreamSlow()
{

    if (CurrentState == EZombieState::Dead) return;
    if (!IsValid(this)) return;

    if (!PlayerCharacter) return;

    float Distance = FVector::Dist(GetActorLocation(), PlayerCharacter->GetActorLocation());
    if (Distance > ScreamRadius) return;

    AHanPlayerCharacter* Player = Cast<AHanPlayerCharacter>(PlayerCharacter);
    if (!Player) return;

    // 원래 속도 기준으로 50% 감소
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
        Player->GetBaseWalkSpeed() / 2.0f; // CrouchWalkSpeed = BaseWalkSpeed/2
}

void ANurseZombie::OnScreamMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (Montage != ScreamMontage) return;
    if (CurrentState != EZombieState::Screaming) return;

    GetWorld()->GetTimerManager().ClearTimer(ScreamSlowTickHandle);

    // ✅ Speed 0 유지
    GetCharacterMovement()->MaxWalkSpeed = 0.0f;

    AAIController* AIC = Cast<AAIController>(GetController());

    if (bLostSightDuringScream)
    {
        if (AIC && AIC->GetBlackboardComponent())
            AIC->GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), nullptr);
        SetCurrentState(EZombieState::Idle);
    }
    else
    {
        if (AIC && AIC->GetBlackboardComponent())
            AIC->GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), PlayerCharacter);
        SetCurrentState(EZombieState::Idle);
    }

    // ✅ 0.1초 후 ResumeLogic + Speed 복구
    GetWorld()->GetTimerManager().SetTimer(ScreamEndTimerHandle, [this]()
        {
            GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;
            // ← 람다 안에서 직접 GetController()로 새로 가져오기
            AAIController* AIC2 = Cast<AAIController>(GetController());
            if (AIC2 && AIC2->GetBrainComponent())
                AIC2->GetBrainComponent()->ResumeLogic(TEXT("Screaming"));
        }, 0.1f, false);
}

float ANurseZombie::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
    class AController* EventInstigator, AActor* DamageCauser)
{
    if (CurrentState == EZombieState::Dead) return 0.0f;

    if (CurrentState == EZombieState::Screaming)
    {
        GetWorld()->GetTimerManager().ClearTimer(ScreamSlowTickHandle);
        StopAnimMontage(ScreamMontage);

        AAIController* AIC = Cast<AAIController>(GetController());
        if (AIC && AIC->GetBrainComponent())
            AIC->GetBrainComponent()->ResumeLogic(TEXT("Screaming"));

        GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;

        if (AIC && AIC->GetBlackboardComponent())
            AIC->GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), PlayerCharacter);

        SetCurrentState(EZombieState::Idle);
    }

    // ✅ 추가: Hit 몽타주 중 이동 차단
    float Result = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    if (CurrentState == EZombieState::Dead) return Result;

    if (CurrentState == EZombieState::Hit)
    {
        AAIController* AIC = Cast<AAIController>(GetController());
        if (AIC)
        {
            AIC->StopMovement();
            AIC->GetBrainComponent()->PauseLogic(TEXT("Hit"));
        }
        GetCharacterMovement()->MaxWalkSpeed = 0.0f;

        // Hit 몽타주 길이만큼 후 BT 재개
        float HitLength = 0.0f;
        if (HitMontage)
            HitLength = HitMontage->GetPlayLength();

        GetWorld()->GetTimerManager().SetTimer(HitResumeTimerHandle, [this]()
            {
                GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;
                AAIController* AIC2 = Cast<AAIController>(GetController());
                if (AIC2 && AIC2->GetBrainComponent())
                    AIC2->GetBrainComponent()->ResumeLogic(TEXT("Hit"));
            }, HitLength, false);
    }

    return Result;
}
void ANurseZombie::Die()
{
    if (CurrentState == EZombieState::Dead) return;

    // ✅ 상태 먼저 Dead (Tick 차단)
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

    Super::Die(); // ← Base의 Dead 체크는 이미 위에서 통과했으므로 제거 필요
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