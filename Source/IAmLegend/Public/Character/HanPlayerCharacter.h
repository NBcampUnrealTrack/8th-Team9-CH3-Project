#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <InputActionValue.h>
#include "Item/InventoryComponent.h"
#include "BattleLogic/WeaponBase.h" 
#include "BattleLogic/BaseDummyCharacter.h"
#include "HanPlayerCharacter.generated.h"

//전방 선언
class USpringArmComponent;
class UCameraComponent;
class UHanInputConfig;
class UInputMappingContext;
class UInventoryComponent; 
class AWeaponBase;         
class ABaseItemActor;      
class ACraftingVolumeActor;

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
	
	// 현재 은신 중인지 상태를 나타내는 센서 플래그 (좀비 AI 시야 차단용)
	UPROPERTY(BlueprintReadOnly, Category = "Character | Stealth")
	bool bIsStealth = false;

	// 실시간으로 부드럽게 변할 현재 디더링 수치
	UPROPERTY(BlueprintReadOnly, Category = "Character | Stealth")
	float CurrentDitherAlpha = 1.0f;

	// 은신이 켜졌을 때 도달해야하는 목표 디더링 수치 (은신 시 0.1f, 평소 1.0f)
	float TargetDitherAlpha = 1.0f;

	// 무기 장착 데이터 맵 반환 Getter 함수
	const TMap<EWeaponSlot, class AWeaponBase*>& GetWeaponSlots() const { return WeaponSlots; }
	
	// 인벤토리에서 쓸 체력 및 이속 겟터셋터 
	float GetHealth() const { return Health; }
	float GetMaxHealth() const { return MaxHealth; }
	void SetHealth(float NewHealth) { Health = NewHealth; }
	float GetBaseWalkSpeed() const { return BaseWalkSpeed; }
	void SetBaseWalkSpeed(float NewSpeed);
	float MeleeDamageModifier = 1.0f;
	TMap<EWeaponSlot, AWeaponBase*>& GetWeaponSlots() { return WeaponSlots; }
	
	// 은신 쿨타임 진행도 반환
	UFUNCTION(BlueprintCallable, Category = "Character | Stealth")
	float GetStealthCooldownRatio() const;

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
	float SprintWalkSpeed = BaseWalkSpeed*1.75;

	// 앉기 시 이동 속도 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Movement")
	float CrouchWalkSpeed = BaseWalkSpeed/2; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Movement")
	float BaseJumpVelocity = 500.f;

	// --- Camera Settings ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Camera")
	float BaseArmLength = 150.f;
	
	// --- 전투 관련 데이터 (Health) ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Battle")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Battle")
	float MaxHealth = 100.f;

	// --- 무기 관련 변수 ---
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character | Weapon")
	TMap<EWeaponSlot, class AWeaponBase*> WeaponSlots;

	// 기본 제공 무기, 이제 무기 장착은 에셋을 통해서 이루어집니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Weapon")
	TMap<EWeaponSlot, UItemDataAsset*> DefaultWeaponDataAssets;

	EWeaponSlot CurrentWeaponSlot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Weapon")
	class AWeaponBase* EquippedWeapon;

	// 무기 소켓 이름 캐싱
	FName WeaponSocketName;
	FName RootSocketName;
	
	// 공격중인지 판단 - 공격중일때 캐릭터를 멈추기 위해서.
	UPROPERTY(BlueprintReadOnly, Category = "Character | State")
	bool bIsAttacking = false;

	// 현재 상호작용 가능한 아이템 (범위 내에 들어온 아이템)	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Interaction")
	class AActor* TargetItem;

	// 무기에서 넘겨 받을 몽타주 변수
	UPROPERTY()
	class UAnimMontage* CurrentAttack_1_Montage = nullptr;

	UPROPERTY()
	class UAnimMontage* CurrentAttack_2_Montage = nullptr;

	UPROPERTY()
	class UAnimMontage* CurrentReloadMontage = nullptr;

	// 포스트 프로세스 제어용 동적 머티리얼 인스턴스를 담을 변수
	UPROPERTY()
	UMaterialInstanceDynamic* PPStealthDynamicMat;

	//카메라
	bool bLastRotationState = false; // 이전 프레임의 상태를 기억
	
	// 은신 스킬 사용시 카메라 채도 변환
	float CurrentSaturation = 1.0f; // 현재 채도
	float TargetSaturation = 1.0f; // 목표 채도

	// 발차기 연출용 카메라 관련 변수
	FVector DefaultSocketOffset;     // 디폴트 스프링 암 소켓 오프셋 수치를 기억해 둘 변수
	FVector TargetSocketOffset;      // 카메라가 최종적으로 도달해야 하는 목표 오프셋 수치 담을 변수 
	FRotator DefaultControlRotation; // 원래대로 돌아올 때 쓸 기본 각도
	FRotator TargetControlRotation;  // 카메라 회전 제어권을 통제하기 위한 목표 회전값 변수
	
	// --- 조준(FOV) 관련 변수 ---
	float DefaultFOV = 90.f;
	float AimingFOV = 60.f;
	float TargetFOV = DefaultFOV;
	float CurrentFOV = TargetFOV;
	float FOVInterpSpeed = 10.f;

	UPROPERTY(BlueprintReadOnly, Category = "Character | Weapon") // ABP에서 쓸려면 필요
	bool bIsAiming = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	// --- 달리기 변수 ---
	bool bIsSprinting = false;

	// --- 무기 관련 함수 ---
	void StartAttack();
	void StopAttack();

	// 조준 함수 
	void StartAim();
	void StopAim();

	// 발차기 연출용 카메라 줌인 줌아웃 함수
	UFUNCTION(BlueprintCallable, Category = "Character | Camera")
	void PlayKickZoomIn(float InFOV, FVector InOffset, float InSpeed);
	UFUNCTION(BlueprintCallable, Category = "Character | Camera")
	void PlayKickZoomOut();

	// 단검 찌르기 연출용 카메라 줌인 줌아웃 함수
	UFUNCTION(BlueprintCallable, Category = "Character | Camera")
	void PlayDaggerZoomIn();
	UFUNCTION(BlueprintCallable, Category = "Character | Camera")
	void PlayDaggerZoomOut();

	// 은신 모드 함수
	UFUNCTION(BlueprintCallable, Category = "Character | Stealth")
	void ToggleStealthMode();

	// 5초 지났을 때 자동으로 은신을 꺼줄 전용 함수
	void DisableStealthMode();

	// 쿨타임이 완전히 끝났을 때 스킬을 해제해 줄 전용 함수
	void ResetStealthCooldown();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stealth|Sound")  // 은신 사운드 변수
	class USoundBase* FirstBigHeartbeatSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stealth|Sound")  // 은신 중 계속 작게 반복될 사운드 큐
	class USoundBase* LoopingStealthSoundCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stealth|Sound") // 은신 중 사운드를 변경 - 실험용
	class USoundMix* StealthSoundMix;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stealth|Sound") // 한기담 - 마스터 사운드의 자식 클래스로 넣으면?
	class USoundClass* GameWorldSoundClass;

	UPROPERTY()  // 지속 사운드를 은신이 끝난 뒤에 같이 끄기 위해 기억해 둘 
	class UAudioComponent* StealthAudioComp;

	// 5초 뒤에 은신이 알아서 종료되게끔 타이머 핸들
	FTimerHandle StealthTimerHandle;

	// 은신 종료 후 쿨타임 시간을 관리하는 핸들
	FTimerHandle StealthCooldownTimerHandle;

	// 현재 쿨타임 도중인지 감시하는 센서 (true면 Q키 차단)
	bool bIsStealthCooldown = false;

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
	void InputReload(const struct FInputActionValue& Value); // 장전
	void InputChangeWeapon(const struct FInputActionValue& Value); // 무기 변경

	UFUNCTION()
	void InventoryShow(const FInputActionValue& InValue); // 인벤토리 관련
	
	// 아이템 쪽에서 캐릭터의 TargetItem을 설정해주기 위한 Getter/Setter
	void SetTargetItem(class AActor* NewItem) { TargetItem = NewItem; }

	// 조준 상태를 반환하는 함수를 추가했습니다. 
	bool IsAiming() const;

	// 무기 관련 입력 함수를 퍼블릭으로 이동했습니다.
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EquipWeapon(UItemDataAsset* NewWeaponData);
	void UnEquipWeapon(EWeaponSlot Slot, bool bDestroyWeapon);
	void ChangeWeapon(EWeaponSlot NewSlot);

	// 몽타주 재생 함수를 퍼블릭으로 선언했습니다.
	void PlayAttackMontage_1(float InPlayRate = 1.0f);
	void PlayAttackMontage_2();
	void PlayReloadMontage();

	// 현재 무기, 탄약 개수를 알기위해 Getter 추가 - 김민성
	class AWeaponBase* GetEquippedWeapon() const { return EquippedWeapon; }
	class UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

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
