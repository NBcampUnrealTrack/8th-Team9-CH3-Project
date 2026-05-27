#include "Ai/BTTask_BossStrafe.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "AI/Boss_PoliceZombie.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_BossStrafe::UBTTask_BossStrafe()
{
    NodeName = TEXT("Boss Strafe");
    bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_BossStrafe::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIC = OwnerComp.GetAIOwner();
    if (!AIC) return EBTNodeResult::Failed;

    ACharacter* Boss = Cast<ACharacter>(AIC->GetPawn());
    if (!Boss) return EBTNodeResult::Failed;

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) return EBTNodeResult::Failed;

    AActor* Target = Cast<AActor>(BB->GetValueAsObject(TEXT("TargetActor")));
    if (Target)
    {
        AIC->SetFocus(Target);
    }
    AIC->StopMovement();

    if (Target)
    {
        FVector ToTarget = (Target->GetActorLocation() - Boss->GetActorLocation()).GetSafeNormal();
        FRotator LookRot = FRotationMatrix::MakeFromX(ToTarget).Rotator();
        LookRot.Pitch = 0.0f;
        LookRot.Roll = 0.0f;
        Boss->SetActorRotation(LookRot);
    }

    StrafeDirection = FMath::RandBool() ? 1.0f : -1.0f;
    TargetStrafeTime = FMath::FRandRange(MinStrafeTime, MaxStrafeTime);
    ElapsedTime = 0.0f;

    Boss->GetCharacterMovement()->MaxWalkSpeed = StrafeSpeed;

    // ✅ WalkMontage로 복구
    ABoss_PoliceZombie* BossZombie = Cast<ABoss_PoliceZombie>(Boss);
    if (BossZombie && BossZombie->WalkMontage)
    {
        BossZombie->PlayAnimMontage(BossZombie->WalkMontage);
    }

    return EBTNodeResult::InProgress;
}

void UBTTask_BossStrafe::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AAIController* AIC = OwnerComp.GetAIOwner();
    if (!AIC) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

    ACharacter* Boss = Cast<ACharacter>(AIC->GetPawn());
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!Boss || !BB) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

    AActor* Target = Cast<AActor>(BB->GetValueAsObject(TEXT("TargetActor")));
    if (!Target) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

    FVector ToTarget = (Target->GetActorLocation() - Boss->GetActorLocation()).GetSafeNormal();
    FRotator LookRot = FRotationMatrix::MakeFromX(ToTarget).Rotator();
    LookRot.Pitch = 0.0f;
    LookRot.Roll = 0.0f;
    Boss->SetActorRotation(FMath::RInterpTo(Boss->GetActorRotation(), LookRot, DeltaSeconds, 10.0f));

    FVector RightVec = FVector::CrossProduct(FVector::UpVector, ToTarget).GetSafeNormal();
    Boss->AddMovementInput(RightVec, StrafeDirection);

    ABoss_PoliceZombie* BossZombie = Cast<ABoss_PoliceZombie>(Boss);
    if (BossZombie && BossZombie->WalkMontage)
    {
        UAnimInstance* AnimInst = Boss->GetMesh()->GetAnimInstance();
        if (AnimInst && !AnimInst->Montage_IsPlaying(BossZombie->WalkMontage))
        {
            BossZombie->PlayAnimMontage(BossZombie->WalkMontage);
        }
    }

    ElapsedTime += DeltaSeconds;
    if (ElapsedTime >= TargetStrafeTime)
    {
        if (BossZombie && BossZombie->WalkMontage)
        {
            BossZombie->StopAnimMontage(BossZombie->WalkMontage);
        }
        Boss->GetCharacterMovement()->MaxWalkSpeed = 0.0f;
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}