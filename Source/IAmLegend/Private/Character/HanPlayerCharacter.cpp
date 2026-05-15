#include "Character/HanPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Input/HanInputConfig.h"
#include "BattleLogic/WeaponBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Item/BaseItemActor.h"

AHanPlayerCharacter::AHanPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// 인벤토리 컴포넌트 생성
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	// 체력
	MaxHealth = 100.f;
	Health = MaxHealth;
	// 초기값 설정
	DefaultFOV = 90.f;
	AimingFOV = 60.f;
	TargetFOV = DefaultFOV;
	CurrentFOV = TargetFOV;
	FOVInterpSpeed = 10.f;
	bIsAiming = false;

	//몸체(캡슐) 크기 설정
	float CharacterHalfHeight = 90.f;
	float CharacterRadius = 40.f;
	GetCapsuleComponent()->InitCapsuleSize(CharacterRadius, CharacterHalfHeight);

	//메쉬의 위치와 회전 설정
	FVector PivotPosition(0.f, 0.f, -CharacterHalfHeight);
	FRotator PivotRotation(0.f, -90.f, 0.f);
	GetMesh()->SetRelativeLocationAndRotation(PivotPosition, PivotRotation);

	//캐릭터 무브먼트 컴포넌트 설정
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed; 
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchWalkSpeed;
	GetCharacterMovement()->JumpZVelocity = BaseJumpVelocity;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	//3인칭 시점 카메라 설정
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = BaseArmLength;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	// 컨트롤러의 회전(마우스)에 따라 캐릭터 몸통이 바로 돌아가는 것을 끕니다.
	bUseControllerRotationYaw = false;

	// 이동하는 방향으로 캐릭터 몸통이 부드럽게 회전하도록 설정합니다.
	GetCharacterMovement()->bOrientRotationToMovement = true;
	// 회전 속도를 조절하고 싶다면 (숫자가 높을수록 빨리 돌았습니다.)
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);

	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

	
}

void AHanPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// 시작 시 무기 소환
	if (WeaponClass) EquipWeapon();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (IsValid(PlayerController) == true)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (IsValid(Subsystem) == true)
		{
			Subsystem->AddMappingContext(PlayerCharacterInputMappingContext, 0);
		}
	}
}

void AHanPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 부드러운 카메라 줌인/아웃 로직 (InterpTo)
	if (CameraComponent)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaTime, FOVInterpSpeed);
		CameraComponent->SetFieldOfView(CurrentFOV);
	}

	// 현재 속도 및 상태 체크
	float CurrentSpeedSq = GetVelocity().SizeSquared();
	bool bIsMoving = CurrentSpeedSq > 100.f;
	// 달리기는 속도가 300 이상일 때라고 가정 
	bool bIsRunning = CurrentSpeedSq > 110000.f; 

	// 회전 모드 스위칭
	// 조준 중이거나 '걷는 중'일 때만 카메라 방향을 본다
	bool bShouldLookCamera = (bIsMoving && !bIsRunning) || bIsAiming;

	if (bShouldLookCamera != bLastRotationState)
	{
		// 걷기: 카메라 방향을 보며 꽃게걸음
		GetCharacterMovement()->bUseControllerDesiredRotation = bShouldLookCamera;

		// 달리기: 가는 방향으로 몸을 돌려 전력 질주
		GetCharacterMovement()->bOrientRotationToMovement = !bShouldLookCamera && bIsRunning;

		bLastRotationState = bShouldLookCamera;
	}
}


void AHanPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	SetViewMode(EViewMode::BackView);
}

void AHanPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (IsValid(EnhancedInputComponent) == true)
	{
		// 이동과 시점 입력 바인딩
		EnhancedInputComponent->BindAction(
			PlayerCharacterInputConfig->Move,
			ETriggerEvent::Triggered,
			this,
			&AHanPlayerCharacter::InputMove 
		);

		EnhancedInputComponent->BindAction(
			PlayerCharacterInputConfig->Look,
			ETriggerEvent::Triggered,
			this,
			&AHanPlayerCharacter::InputLook
		);

		// 달리기 입력 바인딩
		EnhancedInputComponent->BindAction(
			PlayerCharacterInputConfig->Sprint, 
			ETriggerEvent::Started, 
			this, 
			&AHanPlayerCharacter::InputSprintStart
		);

		EnhancedInputComponent->BindAction(
			PlayerCharacterInputConfig->Sprint,
			ETriggerEvent::Completed, 
			this, 
			&AHanPlayerCharacter::InputSprintEnd
		);

		// 점프 입력 바인딩
		EnhancedInputComponent->BindAction(
			PlayerCharacterInputConfig->Jump, 
			ETriggerEvent::Started, 
			this, 
			&ACharacter::Jump
		);
		
		EnhancedInputComponent->BindAction(
			PlayerCharacterInputConfig->Jump, 
			ETriggerEvent::Completed, 
			this, 
			&ACharacter::StopJumping
		);

		EnhancedInputComponent->BindAction(
			PlayerCharacterInputConfig->Crouch,
			ETriggerEvent::Started,
			this,
			&AHanPlayerCharacter::InputCrouchToggle 
		);

		// 공격 
		EnhancedInputComponent->BindAction(
			PlayerCharacterInputConfig->Attack,
			ETriggerEvent::Started,
			this,
			&AHanPlayerCharacter::StartAttack
		);

		EnhancedInputComponent->BindAction(
			PlayerCharacterInputConfig->Attack,
			ETriggerEvent::Completed,
			this,
			&AHanPlayerCharacter::StopAttack
		);

		// 조준 
		EnhancedInputComponent->BindAction(
			PlayerCharacterInputConfig->Aim, 
			ETriggerEvent::Started, 
			this, 
			&AHanPlayerCharacter::StartAim
		);

		EnhancedInputComponent->BindAction(
			PlayerCharacterInputConfig->Aim, 
			ETriggerEvent::Completed, 
			this, 
			&AHanPlayerCharacter::StopAim
		);

		// 인벤토리, 상호작용
		EnhancedInputComponent->BindAction(
			PlayerCharacterInputConfig->Inven, 
			ETriggerEvent::Started, 
			this, 
			&AHanPlayerCharacter::InventoryShow
		);

		EnhancedInputComponent->BindAction(
			PlayerCharacterInputConfig->Interact, 
			ETriggerEvent::Started, 
			this, 
			&AHanPlayerCharacter::InputInteract
		);
	}
}

void AHanPlayerCharacter::InputMove(const FInputActionValue& InValue)
{
	FVector2D MovementVector = InValue.Get<FVector2D>();

	switch (CurrentViewMode)
	{
	case EViewMode::BackView:
	{ 
		// 컨트롤러의 회전을 가져와서 Yaw만 사용하여 회전 계산
		const FRotator ControlRotation = GetController()->GetControlRotation();
		const FRotator ControlRotationYaw(0.f, ControlRotation.Yaw, 0.f);

		// 컨트롤러의 회전을 기준으로 전방과 오른쪽 벡터 계산
		const FVector ForwardVector = FRotationMatrix(ControlRotationYaw).GetUnitAxis(EAxis::X);
		const FVector RightVector = FRotationMatrix(ControlRotationYaw).GetUnitAxis(EAxis::Y);

		// 이동 입력을 전방과 오른쪽 벡터에 적용하여 캐릭터 이동
		AddMovementInput(ForwardVector, MovementVector.X);
		AddMovementInput(RightVector, MovementVector.Y);

		break;
	}

	case EViewMode::None:
	case EViewMode::End:
	default:
		// 현재 뷰 모드에 따라 이동 입력 처리
		AddMovementInput(GetActorForwardVector(), MovementVector.X);
		AddMovementInput(GetActorRightVector(), MovementVector.Y);
		break;
	}
}

// 뷰 모드 설정 함수
void AHanPlayerCharacter::SetViewMode(EViewMode InViewMode)
{
	if (CurrentViewMode == InViewMode)
	{
		return;
	}

	CurrentViewMode = InViewMode;

	switch (CurrentViewMode)
	{
	case EViewMode::BackView:
		// 캐릭터 몸통은 카메라를 따라가지 않게 
		bUseControllerRotationYaw = false;

		// 스프링암 설정 
		SpringArmComponent->TargetArmLength = BaseArmLength;

		// 이 옵션이 켜져 있어야 마우스를 돌릴 때 '스프링암'이 회전한다.
		SpringArmComponent->bUsePawnControlRotation = true;

		// 속도 지연
		SpringArmComponent->bInheritPitch = true;
		SpringArmComponent->bInheritYaw = true;
		SpringArmComponent->bInheritRoll = true;

		// 카메라 지연
		SpringArmComponent->bEnableCameraLag = true;
		SpringArmComponent->CameraLagSpeed = 15.0f;

		// 회전 지연도 켜주면 마우스를 멈춰도 카메라가 부드럽게 멈춘다.
		SpringArmComponent->bEnableCameraRotationLag = true;
		SpringArmComponent->CameraRotationLagSpeed = 10.0f;

		SpringArmComponent->bDoCollisionTest = true;
		break;

	case EViewMode::None:
	case EViewMode::End:
	default:
		break;
	}
}

// 시점 입력 처리 함수
void AHanPlayerCharacter::InputLook(const FInputActionValue& InValue)
{
	FVector2D LookVector = InValue.Get<FVector2D>();

	// 0.5를 곱해서 마우스 감도를 절반으로 - 테스트용
	// 숫자가 작을수록 마우스를 많이 움직여야 화면이 돌아가기에 약간 묵직한 느낌
	float Sensitivity = 0.5f;

	if (CurrentViewMode == EViewMode::BackView)
	{
		AddControllerYawInput(LookVector.X * Sensitivity);
		AddControllerPitchInput(LookVector.Y * Sensitivity);
	}
}

void AHanPlayerCharacter::InputSprintStart(const FInputActionValue& InValue)
{
	// 조준중이라면 달리기 불가능
	if (bIsAiming) return;
	// 현재 앉아있는 상태(bIsCrouched)라면 함수를 종료해서 달리기를 막습니다.
	if (bIsCrouched) return;

	// 달리기 속도 적용
	bIsSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed = SprintWalkSpeed;
}

void AHanPlayerCharacter::InputSprintEnd(const FInputActionValue& InValue)
{
	// 기본 걷기 속도 복원
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

	if (bIsAiming)
	{
		// 조준 중일 때는 느린 속도 유지
		GetCharacterMovement()->MaxWalkSpeed = CrouchWalkSpeed;
	}
	else
	{
		// 조준 중이 아닐 때만 원래 걷기 속도로 복구
		GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	}
}

void AHanPlayerCharacter::InputCrouchToggle(const FInputActionValue& InValue)
{
	if (GetCharacterMovement()->IsFalling())
	{
		return;
	}

	//  이미 앉아있는 상태라면 무조건 일어서기 시도
	if (bIsCrouched)
	{
		UE_LOG(LogTemp, Warning, TEXT("Executing UnCrouch()!"));
		UnCrouch();
	}
	// 서 있는 상태라면 '앉을 수 있는지' 확인 후 앉기
	else
	{
		if (CanCrouch() == true)
		{
			UE_LOG(LogTemp, Warning, TEXT("Executing Crouch()!"));
			Crouch();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("CanCrouch() returned False! Check NavAgent settings."));
		}
	}
}

void AHanPlayerCharacter::InputInteract(const FInputActionValue& InValue)
{
	if (TargetItem)
	{
		// 아이템의 Interact 호출 -> 내부적으로 ApplyPickup(this) 실행됨
		TargetItem->Interact(this);

		// 아이템을 주웠으므로 참조 제거 (Destroy될 것이지만 안전을 위해)
		TargetItem = nullptr;
	}
}

void AHanPlayerCharacter::InventoryShow(const FInputActionValue& InValue)
{
	if (InventoryComponent)
	{
		//BaseItemActor에 있는 ShowInventory 호출
		InventoryComponent->ShowInventory();
	}
}

// ----- 전투 로직 함수 -----
float AHanPlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Health <= 0.f) return 0.f;

	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (ActualDamage > 0.f)
	{
		Health = FMath::Clamp(Health - ActualDamage, 0.f, MaxHealth);
		UE_LOG(LogTemp, Warning, TEXT("현재 체력: %f"), Health); //로그 추가

		if (Health <= 0.f)
		{
			Die();
		}
	}
	return ActualDamage;
}

void AHanPlayerCharacter::Die()
{
	// 나중에 여기에 사망 애니메이션을 넣을수도 있을것같습니다! - 한기담
	Destroy();
}

void AHanPlayerCharacter::EquipWeapon()
{
	if (EquippedWeapon) UnEquipWeapon();

	if (WeaponClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		// 무기 생성
		AWeaponBase* SpawnedWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, SpawnParams);
		if (SpawnedWeapon)
		{
			FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
			SpawnedWeapon->AttachToComponent(GetMesh(), AttachmentRules, FName("WeaponSocket"));
			EquippedWeapon = SpawnedWeapon;
		}
	}
}

void AHanPlayerCharacter::UnEquipWeapon()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->DestroyWeapon(); 
		EquippedWeapon = nullptr;
	}
}

void AHanPlayerCharacter::StartAttack()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->StartWeaponAttack(); // 일반 공격
	}
}

void AHanPlayerCharacter::StopAttack()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->StopWeaponAttack(); // 공격 종료
	}
}

void AHanPlayerCharacter::StartAim() 
{ 
	bIsAiming = true; 
	TargetFOV = AimingFOV; 

	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->MaxWalkSpeed = CrouchWalkSpeed;
	
	SpringArmComponent->SocketOffset = FVector(10.f, 50.f, 0.f);
	
	// 임시로 만들어봄 - 근접 무기일경우 카메라 확대는 안하도록
	if (WeaponClass && WeaponClass->GetName().Contains(TEXT("MeleeWeaponBase")))
	{
		TargetFOV = DefaultFOV; // 줌 안 함
	}
	
}

void AHanPlayerCharacter::StopAim() 
{ 
	bIsAiming = false; 
	TargetFOV = DefaultFOV; 

	// 조준 풀면 다시 입력 방향대로 자유롭게 몸을 돌린다.
	bUseControllerRotationYaw = false;
	//GetCharacterMovement()->bOrientRotationToMovement = true;

	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed; // 조준을 풀었으니 원래 속도로
	SpringArmComponent->SocketOffset = FVector(0.f, 50.f, 0.f);
}

// 조준 상태 반환 함수를 추가했습니다.
bool AHanPlayerCharacter::IsAiming() const
{
	return bIsAiming;
}

// 테스트용 어택 함수 - 공격 몽타주 실행용
void AHanPlayerCharacter::Attack()
{
	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	if (!AnimInst) return;

	AHanPlayerCharacter* MyOwner = Cast<AHanPlayerCharacter>(GetOwner());
	
	if (AnimInst->IsAnyMontagePlaying()) { return; }

	// 조준 중일 때 찌르기 공격 가능
	if (bIsAiming)
	{
		if (AnimInst && !AnimInst->Montage_IsPlaying(KnifeAttack_2))
		{
			UE_LOG(LogTemp, Warning, TEXT("단검 조준 공격 몽타주 실행"));
			PlayAnimMontage(KnifeAttack_2);
		}
	}
	// 그게 아니라면 기본 공격
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("단검 기본 공격 몽타주 실행"));
		PlayAnimMontage(KnifeAttack_1);
	}
}