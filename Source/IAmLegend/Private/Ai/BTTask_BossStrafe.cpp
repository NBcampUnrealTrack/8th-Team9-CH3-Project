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
        AIC->SetFocus(Target); // 추가
    }
    AIC->StopMovement();

    // 플레이어 방향으로 즉시 정렬
    if (Target)
    {
        FVector ToTarget = (Target->GetActorLocation() - Boss->GetActorLocation()).GetSafeNormal();
        FRotator LookRot = FRotationMatrix::MakeFromX(ToTarget).Rotator();
        LookRot.Pitch = 0.0f;
        LookRot.Roll = 0.0f;
        Boss->SetActorRotation(LookRot);
    }

    // Walk 몽타주 먼저 실행
    ABoss_PoliceZombie* BossZombie = Cast<ABoss_PoliceZombie>(Boss);
    if (BossZombie && BossZombie->WalkMontage)
    {
        BossZombie->PlayAnimMontage(BossZombie->WalkMontage);
    }

    // 0.1초 후 이동 부여
    StrafeDirection = FMath::RandBool() ? 1.0f : -1.0f;
    TargetStrafeTime = FMath::FRandRange(MinStrafeTime, MaxStrafeTime);
    ElapsedTime = 0.0f;

    Boss->GetCharacterMovement()->MaxWalkSpeed = StrafeSpeed;

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

    // 플레이어 방향으로 회전
    FVector ToTarget = (Target->GetActorLocation() - Boss->GetActorLocation()).GetSafeNormal();
    FRotator LookRot = FRotationMatrix::MakeFromX(ToTarget).Rotator();
    LookRot.Pitch = 0.0f;
    LookRot.Roll = 0.0f;
    Boss->SetActorRotation(FMath::RInterpTo(Boss->GetActorRotation(), LookRot, DeltaSeconds, 10.0f));

    // 기존: Boss->GetActorRightVector() → 회전 적용 전 벡터
    // 변경: ToTarget 기준 실시간 Right Vector
    FVector RightVec = FVector::CrossProduct(FVector::UpVector, ToTarget).GetSafeNormal();
    Boss->AddMovementInput(RightVec, StrafeDirection);

    ElapsedTime += DeltaSeconds;
    if (ElapsedTime >= TargetStrafeTime)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}