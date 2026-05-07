// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleLogic/DummyPlayerCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "BattleLogic/WeaponBase.h"

ADummyPlayerCharacter::ADummyPlayerCharacter()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->AddLocalOffset(FVector(0.f, 30.f, 60.f));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	DefaultFOV = 90.f;
	AimingFOV = 60.f;
	TargetFOV = DefaultFOV;
	CurrentFOV = TargetFOV;
	FOVInterpSpeed = 10.f;
	
	DefaultMappingContext = nullptr;
	MoveAction = nullptr;
	LookAction = nullptr;
	JumpAction = nullptr;
	AttackAction = nullptr;
	AimAction = nullptr;

	bIsAiming = false;

	WeaponClass = nullptr;
	EquippedWeapon = nullptr;
	WeaponSocketName = "WeaponSocket";
	
}

void ADummyPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	TargetFOV = DefaultFOV;
	CurrentFOV = TargetFOV;

	if (Camera)
	{
		Camera->SetFieldOfView(CurrentFOV);
	}

	if(WeaponClass)
	{
		EquipWeapon();
	}
}

void ADummyPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (Camera)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaTime, FOVInterpSpeed);
		Camera->SetFieldOfView(CurrentFOV);
	}
}

void ADummyPlayerCharacter::EquipWeapon()
{
	if(EquippedWeapon)
	{
		UnEquipWeapon();
	}

	if (WeaponClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		AWeaponBase* SpawnedWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass, SpawnParams);
		if (SpawnedWeapon)
		{
			FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
			SpawnedWeapon->AttachToComponent(GetMesh(), AttachmentRules, WeaponSocketName);
			EquippedWeapon = SpawnedWeapon;
		}
	}
}

void ADummyPlayerCharacter::UnEquipWeapon()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->DestroyWeapon();
		EquippedWeapon = nullptr;
	}
}

void ADummyPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADummyPlayerCharacter::Move);
		}

		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADummyPlayerCharacter::Look);
		}

		if (JumpAction)
		{
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ADummyPlayerCharacter::StartJump);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ADummyPlayerCharacter::StopJump);
		}

		if (AttackAction)
		{
			EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ADummyPlayerCharacter::StartAttack);
			EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &ADummyPlayerCharacter::StopAttack);
		}

		if (AimAction)
		{
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ADummyPlayerCharacter::StartAim);
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ADummyPlayerCharacter::StopAim);
		}
	}
}

void ADummyPlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ADummyPlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
void ADummyPlayerCharacter::StartJump()
{
	Jump();
}

void ADummyPlayerCharacter::StopJump()
{
	StopJumping();
}

void ADummyPlayerCharacter::StartAttack()
{
	if(EquippedWeapon)
	{
		if(!bIsAiming) EquippedWeapon->StartWeaponAttack();
		else EquippedWeapon->StartSubAttack();
	}
}

void ADummyPlayerCharacter::StopAttack()
{
	if(EquippedWeapon)
	{
		if(!bIsAiming) EquippedWeapon->StopWeaponAttack();
		else EquippedWeapon->StopSubAttack();
	}
}
	
void ADummyPlayerCharacter::StartAim()
{
	bIsAiming = true;
	TargetFOV = AimingFOV;
}

void ADummyPlayerCharacter::StopAim()
{
	bIsAiming = false;
	TargetFOV = DefaultFOV;
}


