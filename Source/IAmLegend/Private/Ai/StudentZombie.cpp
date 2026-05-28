// Fill out your copyright notice in the Description page of Project Settings.


#include "Ai/StudentZombie.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AStudentZombie::AStudentZombie()
{
    AttackCooldown = 4.0f;
    Health = 100.0f;
    AttackRange = 150.0f;

    // ✅ 어택 스피어를 머리 소켓으로 이동
    if (AttackSphere)
    {
        AttackSphere->SetupAttachment(GetMesh(), FName("Head"));
    }
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = false;

}

void AStudentZombie::PlayAttackMontage()
{
    if (CurrentState == EZombieState::Dead) return;
    if (!AttackMontage || bIsAttacking) return;

    bIsAttacking = true;
    CurrentAttackInstance++;

    GetCharacterMovement()->StopMovementImmediately();
    GetCharacterMovement()->DisableMovement();

    // ? 학생 좀비만 공격 중 회전 차단
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = false;

    UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
    if (AnimInst)
    {
        AnimInst->Montage_Play(AttackMontage);
    }

    GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &ABase_Zombie::ResetAttack,
        AttackCooldown, false);
}

void AStudentZombie::ResetAttack()
{
    if (CurrentState == EZombieState::Dead) return;

    bIsAttacking = false;
    GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
    bHasAppliedDamageThisAttack = false;

    // ? 공격 끝나면 회전 재개
    bUseControllerRotationYaw = true;
}