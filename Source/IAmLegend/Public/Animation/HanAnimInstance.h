#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "HanAnimInstance.generated.h"

class AHanPlayerCharacter;
class UCharacterMovementComponent;
/**
 * 
 */	
UCLASS()
class IAMLEGEND_API UHanAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<AHanPlayerCharacter> OwnerCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCharacterMovementComponent> OwnerCharacterMovement;

	// 이동 관련 데이터
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector Velocity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float GroundSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 bShouldMove : 1;

	// 공중 상태 데이터
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 bIsFalling : 1;

	// 앉기 상태 데이터
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 bIsCrouched : 1;
};
