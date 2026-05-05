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
	GetCharacterMovement()->JumpZVelocity = BaseJumpVelocity;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	//3인칭 시점 카메라 설정
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = BaseArmLength;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
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
		bUseControllerRotationYaw = true;
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
	// 달리기 속도 적용
	GetCharacterMovement()->MaxWalkSpeed = SprintWalkSpeed;
}

void AHanPlayerCharacter::InputSprintEnd(const FInputActionValue& InValue)
{
	// 기본 걷기 속도 복원
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
}