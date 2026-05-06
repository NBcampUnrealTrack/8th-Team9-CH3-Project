#include "Character/HanPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Input/HanInputConfig.h"
#include "Kismet/KismetSystemLibrary.h"

AHanPlayerCharacter::AHanPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

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

	// 1. 컨트롤러의 회전(마우스)에 따라 캐릭터 몸통이 바로 돌아가는 것을 끕니다.
	bUseControllerRotationYaw = false;

	// 2. 이동하는 방향으로 캐릭터 몸통이 부드럽게 회전하도록 설정합니다.
	GetCharacterMovement()->bOrientRotationToMovement = true;
	// 회전 속도를 조절하고 싶다면 (숫자가 높을수록 빨리 돕니다)
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);

	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
}

void AHanPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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
			&AHanPlayerCharacter::InputCrouchToggle  // 우리 함수로 연결
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
		bUseControllerRotationPitch = false;
		bUseControllerRotationYaw = false;
		bUseControllerRotationRoll = false;

		SpringArmComponent->TargetArmLength = BaseArmLength;
		SpringArmComponent->SetRelativeRotation(FRotator::ZeroRotator);

		SpringArmComponent->bUsePawnControlRotation = true;

		SpringArmComponent->bInheritPitch = true;
		SpringArmComponent->bInheritYaw = true;
		SpringArmComponent->bInheritRoll = false;

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

	// 현재 뷰 모드에 따라 시점 입력 처리
	switch (CurrentViewMode)
	{
	case EViewMode::BackView:
		AddControllerYawInput(LookVector.X);
		AddControllerPitchInput(LookVector.Y);
		break;
	case EViewMode::None:
	case EViewMode::End:
	default:
		break;
	}
}

void AHanPlayerCharacter::InputSprintStart(const FInputActionValue& InValue)
{
	// 현재 앉아있는 상태(bIsCrouched)라면 함수를 종료해서 달리기를 막습니다.
	if (bIsCrouched) return;
	// 달리기 속도 적용
	GetCharacterMovement()->MaxWalkSpeed = SprintWalkSpeed;
}

void AHanPlayerCharacter::InputSprintEnd(const FInputActionValue& InValue)
{
	// 기본 걷기 속도 복원
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
}

void AHanPlayerCharacter::InputCrouchToggle(const FInputActionValue& InValue)
{
	if (GetCharacterMovement()->IsFalling())
	{
		return;
	}

	// 1. 이미 앉아있는 상태라면 무조건 일어서기 시도
	if (bIsCrouched)
	{
		UE_LOG(LogTemp, Warning, TEXT("Executing UnCrouch()!"));
		UnCrouch();
	}
	// 2. 서 있는 상태라면 '앉을 수 있는지' 확인 후 앉기
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