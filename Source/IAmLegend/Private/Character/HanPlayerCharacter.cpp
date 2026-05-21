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
#include "Item/CraftingVolumeActor.h"
#include "UI/MainHUD.h"
#include "BattleLogic/Weapon/ThrowableWeaponBase.h"
#include "BattleLogic/Weapon/RangedWeaponBase.h"
#include "WeaponDataAsset.h"
#include "EngineUtils.h"                     
#include "Ai/BaseZombie_Ai.h"                
#include "BehaviorTree/BlackboardComponent.h" 
#include "Components/AudioComponent.h"
#include <Kismet/GameplayStatics.h>

AHanPlayerCharacter::AHanPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// 인벤토리 컴포넌트 생성
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	// 체력
	MaxHealth = 100.f;
	Health = MaxHealth;

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
	
	// 시작 시 무기 장착
	for (const TPair<EWeaponSlot, UItemDataAsset*>& Elem : DefaultWeaponDataAssets)
	{
		UItemDataAsset* WeaponData = Elem.Value;
		if (IsValid(WeaponData))
		{
			EquipWeapon(WeaponData);
		}
	}

	// 기본 무기가 단검 슬롯에 있다면 단검으로 시작
	if(WeaponSlots.Contains(EWeaponSlot::Dagger) && IsValid(WeaponSlots[EWeaponSlot::Dagger]))
	{
		ChangeWeapon(EWeaponSlot::Dagger);
	}

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (IsValid(PlayerController) == true)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (IsValid(Subsystem) == true)
		{
			Subsystem->AddMappingContext(PlayerCharacterInputMappingContext, 0);
		}
	}

	// 카메라의 포스트 프로세스 머티리얼을 동적 인스턴스로 승격
	if (CameraComponent)
	{
		// 카메라에 등록된 포스트 프로세스 머티리얼 배열(WeightedBlendables)을 가져옴.
		TArray<FWeightedBlendable> Blendables = CameraComponent->PostProcessSettings.WeightedBlendables.Array;

		if (Blendables.Num() > 0 && Blendables[0].Object)
		{
			// 등록된 0번째 머티리얼 에셋을 기반으로 동적 머티리얼(Dynamic)을 생성.
			UMaterialInterface* BaseMat = Cast<UMaterialInterface>(Blendables[0].Object);
			if (BaseMat)
			{
				PPStealthDynamicMat = UMaterialInstanceDynamic::Create(BaseMat, this);

				// 생성된 동적 머티리얼을 카메라에 다시 꽂아줌
				CameraComponent->PostProcessSettings.WeightedBlendables.Array[0].Object = PPStealthDynamicMat;
			}
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
		// 기본 박동 오프셋은 0으로 초기화
		float HeartBeatFOVOffset = 0.0f;

		// 은신 카메라 연출을 위한 로직 
		// TargetDitherAlpha가 0.1f라는 것은 현재 은신중이라는 뜻.
		if (FMath::IsNearlyEqual(TargetDitherAlpha, 0.1f, 0.01f))
		{
			// 4 * x * (1 - x) 공식을 활용한 펄스 생성
			float HeartBeatPulse = 4.0f * CurrentDitherAlpha * (1.0f - CurrentDitherAlpha);

			// 펄스 값 범위 보정
			HeartBeatPulse = FMath::Clamp(HeartBeatPulse - 0.3f, 0.0f, 1.0f);

			// 카메라가 순간적으로 좁혀질 강도 설정
			HeartBeatFOVOffset = HeartBeatPulse * -20.0f;
		}

		// 최종 FOV 적용 (기본 보간 FOV + 심장 박동 오프셋)
		CameraComponent->SetFieldOfView(CurrentFOV + HeartBeatFOVOffset);
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

	// 은신 디더링 효과 천천히	 
	CurrentDitherAlpha = FMath::FInterpTo(CurrentDitherAlpha, TargetDitherAlpha, DeltaTime, 5.0f);
	
	int MaterialCount = GetMesh()->GetNumMaterials();
	for (int i = 0; i < MaterialCount; ++i)
	{
		UMaterialInstanceDynamic* DynamicMaterial = GetMesh()->CreateDynamicMaterialInstance(i);
		if (DynamicMaterial)
		{
			DynamicMaterial->SetScalarParameterValue(FName("Alpha"), CurrentDitherAlpha);
		}
	}

	if (EquippedWeapon)
	{
		TArray<UActorComponent*> WeaponComponents;
		EquippedWeapon->GetComponents(WeaponComponents);

		for (UActorComponent* Component : WeaponComponents)
		{
			UMeshComponent* MeshComp = Cast<UMeshComponent>(Component);
			if (MeshComp)
			{
				int WeaponMatCount = MeshComp->GetNumMaterials();
				for (int j = 0; j < WeaponMatCount; ++j)
				{
					UMaterialInstanceDynamic* WeaponDynamicMat = MeshComp->CreateDynamicMaterialInstance(j);
					if (WeaponDynamicMat)
					{
						WeaponDynamicMat->SetScalarParameterValue(FName("Alpha"), CurrentDitherAlpha);
					}
				}
			}
		}
	}

	// 포스트 프로세스 머티리얼 파라미터 실시간 제어
	if (PPStealthDynamicMat)
	{
		// CurrentDitherAlpha를 뒤집어서 흑백 강도로 사용.
		// - 평상시 (CurrentDitherAlpha = 1.0) -> PP_Saturation = 0.0 (원래 컬러 화면)
		// - 은신시 (CurrentDitherAlpha = 0.1) -> PP_Saturation = 0.9 (90% 흑백 회색조)
		float TargetPPSaturation = 1.0f - CurrentDitherAlpha;

		// 머티리얼에서 만든 파라미터 이름("PP_Saturation")에 값을 꽂아준다.
		PPStealthDynamicMat->SetScalarParameterValue(FName("PP_Saturation"), TargetPPSaturation);
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

		// 장전
		EnhancedInputComponent->BindAction(
			PlayerCharacterInputConfig->Reload,
			ETriggerEvent::Started,
			this,
			&AHanPlayerCharacter::InputReload
		);

		EnhancedInputComponent->BindAction(
			PlayerCharacterInputConfig->ChangeWeapon,
			ETriggerEvent::Triggered,
			this,
			&AHanPlayerCharacter::InputChangeWeapon
		);

		// 은신
		EnhancedInputComponent->BindAction(
			PlayerCharacterInputConfig->Stealth,
			ETriggerEvent::Started,
			this,
			&AHanPlayerCharacter::ToggleStealthMode
		);
	}
}

void AHanPlayerCharacter::InputMove(const FInputActionValue& InValue)
{
	if (bIsAttacking == true) return; //공격중이면 리턴

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
		UnCrouch();
	}
	// 서 있는 상태라면 '앉을 수 있는지' 확인 후 앉기
	else
	{
		if (CanCrouch() == true)
		{
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
	//상호작용 대상이 아예 없으면 즉시 종료
	if (!TargetItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("상호작용 대상이 없습니다."));
		return;
	}

	// 대상이 아이템(ABaseItemActor)인 경우
	if (ABaseItemActor* Item = Cast<ABaseItemActor>(TargetItem))
	{
		Item->Interact(this);
		
		TargetItem = nullptr; 
		return;
	}

	// 제작대(ACraftingVolumeActor)인 경우
	if (ACraftingVolumeActor* CraftStation = Cast<ACraftingVolumeActor>(TargetItem))
	{
		CraftStation->OnInteract(this);
		return;
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
	// GameOver 화면 출력을 위해 코드 작성 했습니다 - 김민성
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		AMainHUD* HUD = Cast<AMainHUD>(PC->GetHUD());
		if (HUD)
		{
			// GameOver UI 출력
			HUD->ShowGameOverHUD();
		}
	}

	// Destroy() 실행 전에 캐릭터를 투명하게 하거나 물리 엔진(Ragdoll) 켜기
	// 바로 Destroy()를 하면 카메라까지 즉시 사라져서 GameOver UI가 보기 힘들 수 있음
	GetMesh()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Destroy();
}

void AHanPlayerCharacter::EquipWeapon(UItemDataAsset* NewWeaponData)
{
	if (!NewWeaponData)
	{
		return;
	}
	
	UWeaponDataAsset* WeaponData = Cast<UWeaponDataAsset>(NewWeaponData);
	TSubclassOf<AWeaponBase> NewWeaponClass = WeaponData->WeaponActorClass;

	if (!NewWeaponClass) return;

	// 이미 해당 슬롯에 무기가 존재한다면 기존 무기를 제거
	EWeaponSlot NewSlot = WeaponData->WeaponSlot;
	if (WeaponSlots.Contains(NewSlot))
	{
		// 기존 무기는 인벤토리에 추가됩니다.
		UnEquipWeapon(NewSlot);
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	// 무기 생성 (데이터 에셋을 등록하기 위해 지연 스폰을 사용)
	AWeaponBase* SpawnedWeapon = GetWorld()->SpawnActorDeferred<AWeaponBase>(
		NewWeaponClass,
		FTransform::Identity,
		this,
		GetInstigator(),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);

	if (SpawnedWeapon)
	{
		// 생성된 무기에 데이터 에셋의 정보를 전달 및 초기화
		SpawnedWeapon->ItemData = NewWeaponData;
		SpawnedWeapon->WeaponInitFromData();
		SpawnedWeapon->FinishSpawning(FTransform::Identity);

		WeaponSlots.Add(NewSlot, SpawnedWeapon);

		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
		SpawnedWeapon->AttachToComponent(GetMesh(), AttachmentRules, FName("WeaponSocket"));

		// 무기는 처음에 보이지 않게 설정
		SpawnedWeapon->SetActorHiddenInGame(true);
		SpawnedWeapon->SetActorEnableCollision(false);

		// 무기 슬롯에 무기 장착 시, 인벤토리에서 아이템 삭제
		if (InventoryComponent)
		{
			// (※ InventoryComponent에 구현된 아이템 제거 함수명에 맞춰 수정해 주세요. 예: RemoveItem)
			InventoryComponent->RemoveItemQuantity(NewWeaponData, 1);
		}

		// 현재 장착된 무기가 없을 때만 새로 장착한 무기로 변경
		if (CurrentWeaponSlot == EWeaponSlot::None)
		{
			ChangeWeapon(NewSlot);
		}
	}
}

void AHanPlayerCharacter::UnEquipWeapon(EWeaponSlot RemoveSlot)
{
	// 슬롯에 무기가 존재하지 않으면 리턴
	if (!WeaponSlots.Contains(RemoveSlot))
	{
		return;
	}

	AWeaponBase* WeaponToRemove = WeaponSlots[RemoveSlot];

	if(WeaponToRemove)
	{
		if (CurrentWeaponSlot == RemoveSlot)
		{
			CurrentWeaponSlot = EWeaponSlot::None;
			EquippedWeapon = nullptr;

			UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
			if (AnimInst)
			{
				CurrentAttack_1_Montage = nullptr;
				CurrentAttack_2_Montage = nullptr;
				CurrentReloadMontage = nullptr;
			}
		}

		// 무기 해제 시 인벤토리에 아이템 데이터 추가
		InventoryComponent->AddItem(WeaponToRemove->ItemData);

		WeaponToRemove->DestroyWeapon(); 
		WeaponSlots.Remove(RemoveSlot);

		// 기본 무기가 단검 슬롯에 있다면 단검으로 시작
		if (WeaponSlots.Contains(EWeaponSlot::Dagger) && IsValid(WeaponSlots[EWeaponSlot::Dagger]))
		{
			ChangeWeapon(EWeaponSlot::Dagger);
		}
	}
}

void AHanPlayerCharacter::ChangeWeapon(EWeaponSlot NewSlot)
{
	// 이미 장착된 슬롯이거나 해당 슬롯에 무기가 없으면 리턴
	if (CurrentWeaponSlot == NewSlot || !WeaponSlots.Contains(NewSlot)) 
	{
		return;
	}

	// 장착된 무기를 숨김
	if (EquippedWeapon)
	{
		EquippedWeapon->SetActorHiddenInGame(true);
		EquippedWeapon->SetActorEnableCollision(false);
	}

	// 새 무기를 장착
	AWeaponBase* NewWeapon = WeaponSlots[NewSlot];
	if(NewWeapon)
	{
		NewWeapon->SetActorHiddenInGame(false);

		EquippedWeapon = NewWeapon;
		CurrentWeaponSlot = NewSlot;
		// 한기담 - 무기가 가진 몽타주 변수들을 캐릭터로 가져옵니다. 

		CurrentAttack_1_Montage = EquippedWeapon->Attack_1_Montage;
		CurrentAttack_2_Montage = EquippedWeapon->Attack_2_Montage;
		CurrentReloadMontage = EquippedWeapon->Reload_Montage;
	}
}

void AHanPlayerCharacter::StartAttack()
{
	if (GetCharacterMovement() && GetCharacterMovement()->IsFalling()) return; // 점프중이면 리턴
	if (bIsCrouched) return; //앉아있는 상태면 리턴

	if (EquippedWeapon)
	{
		EquippedWeapon->StartWeaponAttack(); // 일반 공격
	}
}

void AHanPlayerCharacter::StopAttack()
{
	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	if (!AnimInst) return;

	if (EquippedWeapon)
	{
		EquippedWeapon->StopWeaponAttack(); // 공격 종료
	}
}

void AHanPlayerCharacter::StartAim() 
{ 
	// 장전중일때는 조준 입력 무시
	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	if (AnimInst && AnimInst->IsSlotActive(FName("ReloadSlot"))){ return; }

	bIsAiming = true; 
	TargetFOV = AimingFOV; 

	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->MaxWalkSpeed = CrouchWalkSpeed;
	
	// 근접 무기일경우 조준 모드에서도 카메라 확대는 안하도록
	if (EquippedWeapon == nullptr) return;
	EWeaponType CurrentWeapon = EquippedWeapon->GetWeaponType();
	if (CurrentWeapon == EWeaponType::Dagger || CurrentWeapon == EWeaponType::TwoHandedMelee)
	{
		TargetFOV = DefaultFOV; // 줌 안 함
	}
	
	// 차재현 - 투척 무기 조준시 궤적 표시를 추가했습니다.
	if(AThrowableWeaponBase* ThrowableWeapon = Cast<AThrowableWeaponBase>(EquippedWeapon))
	{
		ThrowableWeapon->EnableTrajectory(true); // 투척 무기라면 조준할 때 궤적 표시
	}
}

void AHanPlayerCharacter::StopAim() 
{ 
	bIsAiming = false; 
	TargetFOV = DefaultFOV; 

	// 조준 풀면 다시 입력 방향대로 자유롭게 몸을 돌린다.
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed; // 조준을 풀었으니 원래 속도로

	// 차재현 - 투척 무기 조준시 궤적 표시를 추가했습니다.
	if (AThrowableWeaponBase* ThrowableWeapon = Cast<AThrowableWeaponBase>(EquippedWeapon))
	{
		ThrowableWeapon->EnableTrajectory(false); // 투척 무기라면 조준을 풀 때 궤적 표시 끄기
	}
}

void AHanPlayerCharacter::PlayCameraZoomIn()
{
	TargetFOV = AimingFOV;
}

void AHanPlayerCharacter::PlayCameraZoomOut()
{
	TargetFOV = DefaultFOV;
}

// 은신 스킬 관련 함수들
void AHanPlayerCharacter::ToggleStealthMode()
{
	UE_LOG(LogTemp, Warning, TEXT("은신이 켜짐"));
	if (bIsStealth == true) return; // 이미 은신 중이면 리턴
	if (bIsStealthCooldown == true) return; // 은신이 풀려도 아직 쿨타임 도중이라면 리턴

	bIsStealth = true;
	TargetDitherAlpha = 0.1f; // 은신이 켜지면 캐릭터, 무기 메시 투명화(0.1) 목표 설정
	TargetSaturation = 0.1f; // 은신이 켜지면 카메라 채도를 흑백(0.1) 목표 설정

	// 은신 사운드 (처음 큰 박동, 후에 5초간 낮은 지속 박동)
	if (FirstBigHeartbeatSound) { UGameplayStatics::PlaySound2D(GetWorld(), FirstBigHeartbeatSound, 2.0f, 1.0f, 0.0f); }
	if (LoopingStealthSoundCue) { StealthAudioComp = UGameplayStatics::SpawnSound2D(GetWorld(), LoopingStealthSoundCue, 1.0f, 1.0f, 0.0f); }
	
	// 은신을 켰다면 주변 AI들의 타겟을 강제로 초기화해줍니다.
	if (bIsStealth)
	{
		// 월드에 있는 모든 좀비 AI 컨트롤러를 찾아서 TargetActor를 비웁니다.
		for (TActorIterator<ABaseZombie_Ai> It(GetWorld()); It; ++It)
		{
			ABaseZombie_Ai* ZombieAI = *It;
			if (ZombieAI && ZombieAI->GetBlackboardComponent())
			{
				// 은신을 켰으므로 좀비들의 타겟에서 나를 지워버립니다.
				ZombieAI->GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), nullptr);
			}
		}
	}

	// 5초 뒤에 자동으로 은신을 꺼주는 'DisableStealthMode' 함수 예약.
	GetWorldTimerManager().SetTimer(StealthTimerHandle, this, &AHanPlayerCharacter::DisableStealthMode, 5.0f, false);
}

void AHanPlayerCharacter::DisableStealthMode()
{
	bIsStealth = false;
	TargetDitherAlpha = 1.0f; // 부드럽게 캐릭터와 무기 메시의 은신이 풀리기 시작 (Tick에서 InterpTo 처리)
	TargetSaturation = 1.0f; // 은신 풀리면 카메라 채도를 정상 채도(1.0) 목표 설정

	// 0.5초에 걸쳐서 사운드가 사라짐
	if (StealthAudioComp && StealthAudioComp->IsPlaying()) { StealthAudioComp->FadeOut(0.5f, 0.0f); }

	UE_LOG(LogTemp, Warning, TEXT("은신이 꺼짐. 10초 쿨타임 적용"));

	// 은신이 풀림과 동시에 쿨타임 함수를 실행 해서 10초 동안은 다시 은신 모드로 들어가지 못하게 막음
	bIsStealthCooldown = true;
	GetWorldTimerManager().SetTimer(StealthCooldownTimerHandle, this, &AHanPlayerCharacter::ResetStealthCooldown, 10.0f, false);
}

void AHanPlayerCharacter::ResetStealthCooldown()
{
	bIsStealthCooldown = false; // 쿨타임 끝. 다시 은신 가능
	UE_LOG(LogTemp, Warning, TEXT("다시 은신 사용 가능"));
}

// 조준 상태 반환 함수를 추가했습니다.
bool AHanPlayerCharacter::IsAiming() const
{
	return bIsAiming;
}

void AHanPlayerCharacter::InputReload(const FInputActionValue& Value)
{
	if(ARangedWeaponBase* RangedWeapon = Cast<ARangedWeaponBase>(EquippedWeapon))
	{
		RangedWeapon->Reload();
	}
}

void AHanPlayerCharacter::InputChangeWeapon(const FInputActionValue& Value)
{
	float SlotIndex = Value.Get<float>();
	EWeaponSlot TargetSlot = EWeaponSlot::None;

	if (FMath::IsNearlyEqual(SlotIndex, 1.0f)) TargetSlot = EWeaponSlot::Melee;
	else if (FMath::IsNearlyEqual(SlotIndex, 2.0f)) TargetSlot = EWeaponSlot::Ranged;
	else if (FMath::IsNearlyEqual(SlotIndex, 3.0f)) TargetSlot = EWeaponSlot::Dagger;
	else if (FMath::IsNearlyEqual(SlotIndex, 4.0f)) TargetSlot = EWeaponSlot::Grenade;
	else return; // 유효하지 않은 입력이면 리턴

	ChangeWeapon(TargetSlot);
}


void AHanPlayerCharacter::PlayAttackMontage_1()
{
	bIsAttacking = true;

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->StopMovementImmediately();
	}

	if(CurrentAttack_1_Montage)
	{
		PlayAnimMontage(CurrentAttack_1_Montage);
	}

	// 몽타주가 완전히 끝나는 프레임 타이밍에 센서를 OFF
	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	if (AnimInst)
	{
		FOnMontageEnded MontageEndedDelegate;
		MontageEndedDelegate.BindLambda([this](UAnimMontage* Montage, bool bInterrupted)
			{
				bIsAttacking = false; // 애니메이션 끝나면 다시 이동 가능하게끔
			});
		AnimInst->Montage_SetEndDelegate(MontageEndedDelegate, CurrentAttack_1_Montage);
	}
}

void AHanPlayerCharacter::PlayAttackMontage_2()
{
	if (CurrentAttack_2_Montage)
	{
		PlayAnimMontage(CurrentAttack_2_Montage);
	}
}

void AHanPlayerCharacter::PlayReloadMontage()
{
	if(CurrentReloadMontage)
	{
		PlayAnimMontage(CurrentReloadMontage);
	}
}

void AHanPlayerCharacter::SetBaseWalkSpeed(float NewSpeed)
{
	BaseWalkSpeed = NewSpeed;
    
	// 언리얼 무브먼트 컴포넌트 실시간 속도 변경
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	}
}

