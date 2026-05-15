// Fill out your copyright notice in the Description page of Project Settings.


#include "Ai/Base_Zombie.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "Animation/AnimMontage.h"
#include "Components/AudioComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SphereComponent.h"
#include "Character/HanPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ABase_Zombie::ABase_Zombie()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 1. 컴포넌트 실제 생성
	IdleSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("IdleSoundComponent"));
	// 2. 루트나 메시에 부착 (보통 메시에 붙입니다)
	if (GetMesh())
	{
		IdleSoundComponent->SetupAttachment(GetMesh());
	}
	AttackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackSphere"));

	// 2. 메시의 손 소켓에 부착 (소켓 이름은 리타겟팅한 스켈레톤의 손 이름을 확인하세요)
	// 보통 "hand_r"이나 "WeaponSocket" 등을 사용합니다.
	AttackSphere->SetupAttachment(GetMesh(), FName("RightHand"));

	// 3. 반지름 조절 (좀비 주먹 크기 정도로 설정)
	AttackSphere->SetSphereRadius(90.0f);
	AttackSphere->SetHiddenInGame(true);

	// 4. 처음부터 켜져 있으면 좀비 옆에만 가도 플레이어가 죽으니, 기본은 꺼둡니다.
	AttackSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	AttackSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 플레이어만 감지

	//한기담 - 좀비의 카메라 콜리전을 ignore로 만들어서 카메라가 닿아도 가까워지지않고 뚫고 지나가게 만들었습니다.
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
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
		DefaultMaxWalkSpeed = Movement->MaxWalkSpeed;
		//  회피 관련 세부 설정 (선택 사항)
		Movement->AvoidanceConsiderationRadius = 100.0f; // 회피를 고려할 반경


		// 공중 제어력(Air Control) 제거 (혹시 모를 넉백 시 공중 이동 방지)
		Movement->AirControl = 0.0f;
	}
	if (AttackSphere)
	{
		AttackSphere->OnComponentBeginOverlap.AddDynamic(this, &ABase_Zombie::OnAttackOverlapBegin);
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
	if (AttackMontage && !bIsAttacking) {
		// 1. 공격 시작 시 대기 소리를 즉시 끕니다.
		if (IdleSoundComponent) {
			IdleSoundComponent->FadeOut(0.2f, 0.0f);

			// 혹시 이미 돌아가고 있을지 모를 '소리 켜기 타이머'를 취소합니다.
			GetWorld()->GetTimerManager().ClearTimer(IdleSoundTimerHandle);
		}

		bIsAttacking = true;
		CurrentState = EZombieState::Attacking;

		// 1. 이동 멈추기 및 속도 0으로 설정
		AAIController* AIC = Cast<AAIController>(GetController());
		if (AIC)
		{
			AIC->StopMovement();
		}
		if (AIC && AIC->GetBlackboardComponent())
		{
			// 현재 상태를 블랙보드의 "CurrentState" 키에 전달 (Enum 값 전달)
			AIC->GetBlackboardComponent()->SetValueAsEnum(TEXT("CurrentState"), (uint8)EZombieState::Attacking);
		}
		// CharacterMovement의 최대 속도를 0으로 만들어 물리적 이동을 차단합니다.
		if (GetCharacterMovement())
		{
			GetCharacterMovement()->MaxWalkSpeed = 0.0f;
		}

		PlayAnimMontage(AttackMontage);

		// 2. 공격 쿨타임 타이머 (공격 가능 상태 복구용)
		GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, this, &ABase_Zombie::ResetAttack, AttackCooldown, false);

		// 3. 쿨타임 * 2의 시간 뒤에 대기 소리를 다시 켜는 타이머 설정
		float SoundDelay = AttackCooldown * 2.0f;
		GetWorld()->GetTimerManager().SetTimer(IdleSoundTimerHandle, this, &ABase_Zombie::ResumeIdleSound, SoundDelay, false);
	}
}

// 대기 소리를 다시 켜주는 새로운 함수
void ABase_Zombie::ResumeIdleSound()
{
	if (IdleSoundComponent)
	{
		IdleSoundComponent->FadeIn(0.5f);
	}
}
void ABase_Zombie::ResetAttack() {
	bIsAttacking = false;

	// 1. 속도 복구
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;
	}

	// 2. C++ 상태 먼저 변경 (중요!)
	if (CurrentState != EZombieState::Dead)
	{
		CurrentState = EZombieState::Idle;
	}

	// 3. 변경된 상태를 블랙보드에 반영
	AAIController* AIC = Cast<AAIController>(GetController());
	if (AIC && AIC->GetBlackboardComponent())
	{
		// 이제 C++의 CurrentState와 블랙보드의 값이 완벽히 일치합니다.
		AIC->GetBlackboardComponent()->SetValueAsEnum(TEXT("CurrentState"), (uint8)CurrentState);
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
	if (CurrentState == EZombieState::Dead) return; // 중복 실행 방지
	CurrentState = EZombieState::Dead;

	// 1. 소리 제어
	// 대기 소리 즉시 정지
	if (IdleSoundComponent)
	{
		IdleSoundComponent->Stop();
	}

	// 사망 소리 재생 (헤더에 USoundBase* DeathSound; 가 선언되어 있어야 함)
	if (DeathSound)
	{
		// 액터가 사라져도 소리가 끝까지 나도록 PlaySoundAtLocation 사용
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}

	// 2. 타이머 정리 (공격 중 죽었을 때 소리가 다시 켜지는 것 방지)
	GetWorld()->GetTimerManager().ClearTimer(IdleSoundTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);

	// 3. 죽음 애니메이션 재생
	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}

	// 4. AI 중지 및 연결 해제
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		AIController->StopMovement();
		AIController->UnPossess();
	}

	// 5. 충돌 제거 및 시체 삭제 예약
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(3.0f);
}

void ABase_Zombie::OnAttackOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	// 1. 공격 중인지 먼저 확인
	if (!bIsAttacking) return;

	// 2. 현재 게임 시간 가져오기
	float CurrentTime = GetWorld()->GetTimeSeconds();

	// 3. [핵심] 현재 시간과 마지막 공격 시간의 차이가 2초보다 작으면 즉시 리턴!
	if (CurrentTime - LastDamageTime < DamageLockDuration)
	{
		return;
	}

	if (OtherActor && OtherActor != this)
	{
		AHanPlayerCharacter* Player = Cast<AHanPlayerCharacter>(OtherActor);
		if (Player)
		{
			// 4. 데미지 주기 직전에 현재 시간을 마지막 공격 시간으로 저장 (잠금 시작)
			LastDamageTime = CurrentTime;

			UGameplayStatics::ApplyDamage(Player, AttackDamage, GetController(), this, nullptr);

			// 로그로 확인 (잠금이 걸렸으므로 이제 0번만 찍혀야 함)
			UE_LOG(LogTemp, Warning, TEXT("좀비의 타격 성공!"));

			DisableAttackCollision();
		}
	}
}

// 애니메이션 노티파이에 의해 호출될 함수 (블루프린트에서 호출 가능하게 UFUNCTION 설정 필요)
void ABase_Zombie::EnableAttackCollision()
{
	if (AttackSphere)
	{
		bIsAttacking = true;
		// 공격할 때마다 인스턴스 번호를 올립니다.
		CurrentAttackInstance++;

		AttackSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		// UpdateOverlaps는 여전히 주석 처리 유지하세요!
	}
}
void ABase_Zombie::DisableAttackCollision()
{
	if (AttackSphere)
	{

		AttackSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}