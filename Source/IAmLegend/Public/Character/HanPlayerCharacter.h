#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include <InputActionValue.h>
#include "HanPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UHanInputConfig;
class UInputMappingContext;

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

	// PossessedBy는 캐릭터가 컨트롤러에 의해 제어될 때 호출되는 함수이다. 
	virtual void PossessedBy(AController* NewController) override;
protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
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
	float BaseJumpVelocity = 700.f;

	// --- Camera Settings ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Camera")
	float BaseArmLength = 400.f;
	
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
