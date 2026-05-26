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
#include "BrainComponent.h"
#include "Gamemode/MainGameStateBase.h"
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
	AttackSphere->SetSphereRadius(60.0f);
	AttackSphere->SetHiddenInGame(true);

	// 4. 처음부터 켜져 있으면 좀비 옆에만 가도 플레이어가 죽으니, 기본은 꺼둡니다.
	AttackSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	AttackSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 플레이어만 감지

	//한기담 - 좀비의 카메라 콜리전을 ignore로 만들어서 카메라가 닿아도 가까워지지않고 뚫고 지나가게 만들었습니다.
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCanEverAffectNavigation(false);
	// 대신 아래로 네비 영향 주는 컴포넌트 따로 설정
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Ignore);
	GetMesh()->SetCanEverAffectNavigation(false);  // 이거 추가
	GetCapsuleComponent()->bDynamicObstacle = false;           // 추가
	AttackSphere->SetCanEverAffectNavigation(false);
	GetCapsuleComponent()->SetCollisionProfileName(FName("Enemy"));
	GetMesh()->SetCollisionProfileName(FName("Enemy"));

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
	if (IdleSoundComponent)
	{
		// 거리 감쇠 활성화
		IdleSoundComponent->bAllowSpatialization = true;
		IdleSoundComponent->bOverrideAttenuation = false; // 에셋의 Attenuation 사용
	}
}

//void ABase_Zombie::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//	if (PlayerCharacter && !bIsAttacking)
//	{
//		// GetDistanceTo(두 캐릭터 캡슐 표면 사이의 최단 거리)
//		float Distance = GetDistanceTo(PlayerCharacter);
//		UE_LOG(LogTemp, Warning, TEXT("보스 거리: %f"), Distance);
//		// AI가 AcceptanceRadius=30으로 다가오므로, 공격 범위는 50~80 정도가 적당합니다.
//		// 좀비 애니메이션이 팔을 쭉 뻗는 형태라면 80~100도 괜찮습니다.
//		if (Distance <= AttackRange) // 100.0f에서 조금 더 타이트하게 조정
//		{
//			UE_LOG(LogTemp, Warning, TEXT("공격 시도! Distance: %f, AttackRange: %f"), Distance, AttackRange);
//			PlayAttackMontage();
//		}
//	}
//}
void ABase_Zombie::Tick(float DeltaTime)
{
	if (CurrentState == EZombieState::Dead) return; // ← 추가

	Super::Tick(DeltaTime);

	if (PlayerCharacter && !bIsAttacking)
	{
		float Distance = GetDistanceTo(PlayerCharacter);

		if (Distance <= AttackRange)
		{
			FVector ToPlayer = (PlayerCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			FVector Forward = GetActorForwardVector();
			float DotProduct = FVector::DotProduct(Forward, ToPlayer);

			if (DotProduct > 0.0f)
			{
				PlayAttackMontage();
			}
		}
	}
}
void ABase_Zombie::PlayAttackMontage()
{
	if (CurrentState == EZombieState::Dead) return;
	if (!AttackMontage || bIsAttacking) return;

	bIsAttacking = true;
	CurrentAttackInstance++;

	// ✅ 공격 전 플레이어 방향으로 회전
	if (PlayerCharacter)
	{
		FVector ToPlayer = (PlayerCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		FRotator LookRot = FRotationMatrix::MakeFromX(ToPlayer).Rotator();
		LookRot.Pitch = 0.0f;
		LookRot.Roll = 0.0f;
		SetActorRotation(LookRot);
	}

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	if (AnimInst)
	{
		AnimInst->Montage_Play(AttackMontage);
	}

	GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &ABase_Zombie::ResetAttack,
		AttackCooldown, false);
}
// 대기 소리를 다시 켜주는 새로운 함수
void ABase_Zombie::ResumeIdleSound()
{
	if (IdleSoundComponent)
	{
		IdleSoundComponent->FadeIn(0.5f);
	}
}
void ABase_Zombie::ResetAttack()
{
	if (CurrentState == EZombieState::Dead) return;

	bIsAttacking = false;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	bHasAppliedDamageThisAttack = false;


}
void ABase_Zombie::PlayKnockdownMontage()
{
	if (CurrentState == EZombieState::Dead) return;
	if (!KnockdownMontage) return;

	CurrentState = EZombieState::Knockdown;
	bIsAttacking = true;

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	AAIController* AIC = Cast<AAIController>(GetController());
	if (AIC && AIC->GetBrainComponent())
		AIC->GetBrainComponent()->PauseLogic(TEXT("Knockdown"));

	float MontageLength = PlayAnimMontage(KnockdownMontage);

	GetWorld()->GetTimerManager().SetTimer(HitTimerHandle, [this]()
		{
			if (CurrentState == EZombieState::Dead) return;
			bIsAttacking = false;
			CurrentState = EZombieState::Idle;
			GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

			AAIController* AIC2 = Cast<AAIController>(GetController());
			if (AIC2 && AIC2->GetBrainComponent())
				AIC2->GetBrainComponent()->ResumeLogic(TEXT("Knockdown"));
		}, MontageLength, false);
}
float ABase_Zombie::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (CurrentState == EZombieState::Dead) return 0.0f;

	Health -= DamageAmount;

	if (Health <= 0.0f)
	{
		Die();
	}
	else
	{
		if (HitMontage && CurrentState != EZombieState::Attacking)
		{
			CurrentState = EZombieState::Hit;
			float HitMontageLength = PlayAnimMontage(HitMontage);

			bIsAttacking = true;

			// ✅ Hit 중 이동 차단
			GetCharacterMovement()->StopMovementImmediately();
			GetCharacterMovement()->DisableMovement();

			AAIController* AIC = Cast<AAIController>(GetController());
			if (AIC)
			{
				AIC->StopMovement();
				if (AIC->GetBrainComponent())
					AIC->GetBrainComponent()->PauseLogic(TEXT("Hit"));
			}

			GetWorld()->GetTimerManager().SetTimer(HitTimerHandle, [this]()
				{
					if (CurrentState == EZombieState::Dead) return;
					bIsAttacking = false;
					CurrentState = EZombieState::Idle;

					// ✅ Hit 끝나면 이동 재개
					GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

					AAIController* AIC2 = Cast<AAIController>(GetController());
					if (AIC2 && AIC2->GetBrainComponent())
						AIC2->GetBrainComponent()->ResumeLogic(TEXT("Hit"));
				}, HitMontageLength, false);
		}
	}

	return DamageAmount;
}

void ABase_Zombie::Die()
{
	CurrentState = EZombieState::Dead;

	// ✅ 재생 중인 모든 몽타주 즉시 중단 → ABP State Machine이 Death State로 전환
	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	if (AnimInst)
	{
		AnimInst->StopAllMontages(0.0f);
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->SetComponentTickEnabled(false);

	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetEnableGravity(false);

	GetWorldTimerManager().ClearTimer(DeathTimerHandle);
	GetWorldTimerManager().ClearTimer(DeathFreezeTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(IdleSoundTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);

	if (IdleSoundComponent) IdleSoundComponent->Stop();
	if (DeathSound)
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());

	AttackSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		AIController->StopMovement();
		if (AIController->GetBrainComponent())
		{
			AIController->GetBrainComponent()->StopLogic(TEXT("Dead"));
		}
	}

	DeathAnimLength = 3.0f;

	GetWorld()->GetTimerManager().SetTimer(DeathTimerHandle, [this]()
		{
			SetActorHiddenInGame(true);
			SetLifeSpan(0.1f);
		}, DeathAnimLength + 0.5f, false);

	AMainGameStateBase* GameState = Cast<AMainGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
	if (GameState)
	{
		GameState->AddPlayerKillCount();
	}
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