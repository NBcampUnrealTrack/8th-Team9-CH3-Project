
#include "Animation/HanAnimInstance.h"
#include "Character/HanPlayerCharacter.h" 
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UHanAnimInstance::NativeInitializeAnimation()
{
	APawn* OwnerPawn = TryGetPawnOwner();
	if (IsValid(OwnerPawn) == true)
	{
		OwnerCharacter = Cast<AHanPlayerCharacter>(OwnerPawn);
		if (IsValid(OwnerCharacter))
		{
			OwnerCharacterMovement = OwnerCharacter->GetCharacterMovement();
		}
	}
}

void UHanAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (IsValid(OwnerCharacter) == true && IsValid(OwnerCharacterMovement) == true)
	{
		Velocity = OwnerCharacterMovement->Velocity;
		GroundSpeed = UKismetMathLibrary::VSizeXY(Velocity);

		float GroundAcceleration = UKismetMathLibrary::VSizeXY(OwnerCharacterMovement->GetCurrentAcceleration());
		bool bIsAccelerationNearlyZero = FMath::IsNearlyZero(GroundAcceleration);
		bShouldMove = (KINDA_SMALL_NUMBER < GroundSpeed) && (bIsAccelerationNearlyZero == false);
		
		// 캐릭터가 현재 IsFalling인지 확인
		bIsFalling = OwnerCharacterMovement->IsFalling();

		// 캐릭터가 현재 앉아있는지(bIsCrouched) 확인해서 애니메이션 변수에 저장합니다.
		bIsCrouched = OwnerCharacterMovement->IsCrouching();
	}
}