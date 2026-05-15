#include "Ai/NurseZombie.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ANurseZombie::ANurseZombie()
{
    // 생성자: 이 좀비만의 기본 스탯 설정
    AttackCooldown = 4.0f;
    Health = 100.0f;
    AttackRange = 100.0f;
}
void ANurseZombie::BeginPlay()
{
    Super::BeginPlay();

    // 몽타주 종료 델리게이트 등록
    UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
    if (AnimInst)
    {
        AnimInst->OnMontageEnded.AddDynamic(this, &ANurseZombie::OnScreamMontageEnded);
    }
}
// ---------------------------------------------------------
// [로직 설명]: Tick 함수
// 매개변수 float DeltaTime: 이전 프레임과 현재 프레임 사이의 시간 간격 (초 단위).
// 컴퓨터 사양에 상관없이 일정한 속도로 로직이 흐르게 하기 위해 곱해주는 값입니다.
// ---------------------------------------------------------
void ANurseZombie::Tick(float DeltaTime)
{
    // [중요] 여기서 직접 거리를 계산하지 않습니다! 
    // 거리는 이미 AI Controller의 시야 센서(Perception)가 감시하고 있습니다.

    // 현재 비명을 지르는 중이라면, 부모 좀비의 추격/공격 체크 로직(Super::Tick)이 실행되지 않게 막습니다.
    if (CurrentState == EZombieState::Screaming)
    {
        return;
    }

    // 비명 상태가 아닐 때만 부모(Base_Zombie)의 Tick을 호출하여 일반적인 공격 사거리 체크를 수행합니다.
    Super::Tick(DeltaTime);
}

// ---------------------------------------------------------
// [로직 설명]: 비명 실행 함수
// AI Controller(뇌)가 "플레이어를 발견했으니 비명을 질러라"라고 명령을 내릴 때 호출됩니다.
// ---------------------------------------------------------
void ANurseZombie::PlayScreamMontage()
{
    if (ScreamMontage && CurrentState == EZombieState::Idle)
    {
        SetCurrentState(EZombieState::Screaming);
        bLostSightDuringScream = false; // 초기화

        if (auto* AIC = Cast<AAIController>(GetController()))
            AIC->StopMovement();

        PlayAnimMontage(ScreamMontage);
        // ↑ 몽타주는 1회만 재생, 델리게이트가 끝을 감지
    }
}
void ANurseZombie::OnScreamMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (Montage != ScreamMontage) return;
    if (CurrentState != EZombieState::Screaming) return;

    // 비명이 끝난 후 → 시야 유무에 따라 상태 분기
    if (bLostSightDuringScream)
    {
        // 플레이어를 못 보는 상태 → Idle로 복귀
        SetCurrentState(EZombieState::Idle);
    }
    else
    {
        // 아직 플레이어가 시야에 있음 → 추격 시작
        SetCurrentState(EZombieState::Idle); // BT가 다시 MoveTo를 타도록
    }
}
// ---------------------------------------------------------
// [로직 설명]: 데미지 처리 함수 (언리얼 내장 시스템)
// 매개변수 설명:
// 1. float DamageAmount: 입은 데미지 수치.
// 2. FDamageEvent const& DamageEvent: 어떤 '방식'으로 맞았는지(총알, 폭발 등)에 대한 상세 데이터 구조체.
// 3. AController* EventInstigator: 때린 사람의 '뇌(Controller)'. 누가 공격했는지 식별용.
// 4. AActor* DamageCauser: 때린 '도구(Actor)'. 총알인지 칼인지 식별용.
// ---------------------------------------------------------
float ANurseZombie::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
    // 죽은 상태면 무시
    if (CurrentState == EZombieState::Dead) return 0.0f;

    // [핵심] 비명을 지르는 도중에 맞았다면?
    if (CurrentState == EZombieState::Screaming)
    {
        // 1. 비명 애니메이션을 즉시 멈춥니다.
        StopAnimMontage(ScreamMontage);

        // 2. 상태를 Idle(대기) 혹은 Hit(피격)으로 돌려놓습니다. 
        // 그래야 AI가 다시 "아, 이제 비명 끝났으니 쫓아가야겠다"라고 판단합니다.
        SetCurrentState(EZombieState::Idle);
    }

    // 나머지는 부모 클래스의 데미지 처리(체력 차감, 사망 체크, 피격 애니메이션 등)에 맡깁니다.
    return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

// ---------------------------------------------------------
// [로직 설명]: 상태 설정 함수
// 좀비의 내부 상태(C++)를 바꿀 때, AI의 기억 장치(Blackboard)도 똑같이 업데이트해 줍니다.
// ---------------------------------------------------------
void ANurseZombie::SetCurrentState(EZombieState NewState)
{
    CurrentState = NewState;

    // AI Controller를 가져와서 블랙보드 값을 갱신합니다. (뇌와 몸의 싱크를 맞춤)
    AAIController* AIC = Cast<AAIController>(GetController());
    if (AIC && AIC->GetBlackboardComponent())
    {
        // "CurrentState"라는 이름의 블랙보드 키에 현재 상태(Enum)를 숫자로 저장합니다.
        AIC->GetBlackboardComponent()->SetValueAsEnum(TEXT("CurrentState"), (uint8)NewState);
    }
}