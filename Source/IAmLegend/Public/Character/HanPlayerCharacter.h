#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <InputActionValue.h>
#include "Item/InventoryComponent.h"
#include "HanPlayerCharacter.generated.h"

//전방 선언
class USpringArmComponent;
class UCameraComponent;
class UHanInputConfig;
class UInputMappingContext;
class UInventoryComponent; 
class AWeaponBase;         
class ABaseItemActor;      

//BackView
UENUM(BlueprintType)
enum class EViewMode : uint8
{
	None,
	BackView,
	End
};

UCLASS()
class IAMLEGEND_API AHanPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

#pragma region Override ACharacter

public:
	AHanPlayerCharacter();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override; // Tick에서 FOV를 부드럽게 넘기기위해서
	
	// 데미지를 받았을 때 엔진에서 호출해주는 함수 (오버라이드)
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	// 체력이 0이 되었을 때 실행할 함수
	virtual void Die();

	// PossessedBy는 캐릭터가 컨트롤러에 의해 제어될 때 호출되는 함수이다. 
	virtual void PossessedBy(AController* NewController) override;

	
protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	// 인벤토리 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<UInventoryComponent> InventoryComponent;

	// 카메라 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<UCameraComponent> CameraComponent;

	// --- Movement Settings ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Movement")
	float BaseWalkSpeed = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Movement")
	float SprintWalkSpeed = BaseWalkSpeed*2;

	// 앉기 시 이동 속도 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Movement")
	float CrouchWalkSpeed = BaseWalkSpeed/1.5; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Movement")
	float BaseJumpVelocity = 350.f;

	// --- Camera Settings ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Camera")
	float BaseArmLength = 400.f;
	
	// --- 전투 관련 데이터 (Health) ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Battle")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Battle")
	float MaxHealth = 100.f;

	// --- 무기 관련 변수 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Weapon")
	TSubclassOf<class AWeaponBase> WeaponClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Weapon")
	class AWeaponBase* EquippedWeapon;
	
	// 현재 상호작용 가능한 아이템 (범위 내에 들어온 아이템)	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Interaction")
	class ABaseItemActor* TargetItem;



	// --- 조준(FOV) 관련 변수 ---
	float DefaultFOV = 90.f;
	float AimingFOV = 60.f;
	float TargetFOV = 90.f;
	float CurrentFOV = 90.f;
	float FOVInterpSpeed = 10.f;
	bool bIsAiming = false;

	// --- 무기 관련 함수 ---
	void EquipWeapon();
	void UnEquipWeapon();
	void Attack();

	// 조준 함수 
	void StartAim();
	void StopAim();

#pragma endregion

#pragma region Input

#pragma region ViewMode
public:
	void InputMove(const FInputActionValue& InValue);
	void InputLook(const FInputActionValue& InValue);
	void SetViewMode(EViewMode InViewMode);
	void InputSprintStart(const struct FInputActionValue& InValue); // 달리기 시작
	void InputSprintEnd(const struct FInputActionValue& InValue);   // 달리기 종료
	void InputCrouchToggle(const FInputActionValue& InValue);
	void InputInteract(const FInputActionValue& InValue); // F 키 입력 시 실행될 함수
	
	UFUNCTION()
	void InventoryShow(const FInputActionValue& InValue); // 인벤토리 관련
	
	// 아이템 쪽에서 캐릭터의 TargetItem을 설정해주기 위한 Getter/Setter
	void SetTargetItem(class ABaseItemActor* NewItem) { TargetItem = NewItem; }

	// 조준 상태를 반환하는 함수를 추가했습니다. 
	bool IsAiming() const;

protected:
	/// 현재 시점 모드
	EViewMode CurrentViewMode = EViewMode::None;

	// 입력 액션들이 등록된 데이터 에셋과 입력 매핑 컨텍스트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<UHanInputConfig> PlayerCharacterInputConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	TObjectPtr<UInputMappingContext> PlayerCharacterInputMappingContext;

#pragma endregion	



};
