// Fill out your copyright notice in the Description page of Project Settings.


#include "Ai/Base_Zombie.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ABase_Zombie::ABase_Zombie()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


}

void ABase_Zombie::BeginPlay()
{
    Super::BeginPlay();

    // 게임 시작 시 0번 플레이어(우리 주인공)를 미리 찾아 변수에 담아둡니다.
    PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

    UCharacterMovementComponent* Movement = GetCharacterMovement(); // 최적화를 위해 집어넣습니다. 원하시면 제거 가능
    if (Movement)
    {
        // 점프 불가능하게 설정
        Movement->NavAgentProps.bCanJump = false; //
        // 떨어지는 기능 제거
        Movement->bCanWalkOffLedges = false;
        // 장애물 회피(RVO Avoidance) 활성화
        Movement->bUseRVOAvoidance = true;

        //  회피 관련 세부 설정 (선택 사항)
        Movement->AvoidanceConsiderationRadius = 100.0f; // 회피를 고려할 반경


        // 공중 제어력(Air Control) 제거 (혹시 모를 넉백 시 공중 이동 방지)
        Movement->AirControl = 0.0f;
    }
}

void ABase_Zombie::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (PlayerCharacter && !bIsAttacking)
    {
        // GetDistanceTo(두 캐릭터 캡슐 표면 사이의 최단 거리)
        float Distance = GetDistanceTo(PlayerCharacter);

        // AI가 AcceptanceRadius=30으로 다가오므로, 공격 범위는 50~80 정도가 적당합니다.
        // 좀비 애니메이션이 팔을 쭉 뻗는 형태라면 80~100도 괜찮습니다.
        if (Distance <= AttackRange) // 100.0f에서 조금 더 타이트하게 조정
        {
            PlayAttackMontage();
        }
    }
}
void ABase_Zombie::PlayAttackMontage() {
    // 몽타주 에셋이 존재하고, 현재 공격 중이 아닐 때만 실행합니다.
    if (AttackMontage && !bIsAttacking) {
        bIsAttacking = true;
        CurrentState = EZombieState::Attacking;
        PlayAnimMontage(AttackMontage); // 공격 애니메이션 재생

        // [타이머] AttackCooldown(예: 4초) 시간이 지난 뒤에 ResetAttack 함수를 딱 한 번 실행합니다.
        GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, this, &ABase_Zombie::ResetAttack, AttackCooldown, false);
    }
}

void ABase_Zombie::ResetAttack() {
    bIsAttacking = false;

    // [수정] 죽지 않았다면 상태를 다시 Idle로 돌려놔야 비헤이비어 트리가 다음 행동을 시킵니다.
    if (CurrentState != EZombieState::Dead)
    {
        CurrentState = EZombieState::Idle;
    }
}

float ABase_Zombie::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
    if (CurrentState == EZombieState::Dead) return 0.0f; // 죽은 좀비는 또 죽지 않게 예외 처리

    Health -= DamageAmount; // 체력 차감

    if (Health <= 0.0f)
    {
        Die(); // 체력 없으면 사망
    }
    else
    {
        // 공격 중이 아닐 때만 피격(Hit) 애니메이션을 보여줍니다. (공격 끊김 방지)
        if (HitMontage && CurrentState != EZombieState::Attacking)
        {
            CurrentState = EZombieState::Hit;
            PlayAnimMontage(HitMontage);
            // Tip: 몽타주 종료 시 다시 Idle로 바꾸려면 OnMontageEnded 델리게이트를 쓰면 좋습니다.
        }
    }

    return DamageAmount;
}

void ABase_Zombie::Die()
{
    CurrentState = EZombieState::Dead;

    // 1. 죽음 애니메이션 재생
    if (DeathMontage)
    {
        PlayAnimMontage(DeathMontage);
    }

    // 2. AI 중지: 좀비를 조종하던 두뇌(AIController)를 멈추고 연결을 끊습니다.
    AAIController* AIController = Cast<AAIController>(GetController());
    if (AIController)
    {
        AIController->StopMovement(); // 이동 중지
        AIController->UnPossess();    // 조종 해제
    }

    // 3. 충돌 제거: 죽은 좀비가 벽처럼 가로막지 않도록 캡슐 충돌체를 끕니다.
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 4. 시체 삭제: 죽은 후 3초 뒤에 게임 월드에서 완전히 사라집니다.
    SetLifeSpan(3.0f);
}

