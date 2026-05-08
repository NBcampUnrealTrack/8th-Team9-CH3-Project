// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseDummyCharacter.h"
#include "DummyPlayerCharacter.generated.h"

/**
 * 
 */
class UInputAction;
struct FInputActionValue;

UCLASS()
class IAMLEGEND_API ADummyPlayerCharacter : public ABaseDummyCharacter
{
	GENERATED_BODY()

public:
	ADummyPlayerCharacter();

	bool IsAiming() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float DefaultFOV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float AimingFOV;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float CurrentFOV;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float TargetFOV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float FOVInterpSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bIsAiming;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<class AWeaponBase> WeaponClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	class AWeaponBase* EquippedWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FName WeaponSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* AimAction;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void EquipWeapon();
	void UnEquipWeapon();

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StartJump();
	void StopJump();
	void StartAttack();
	void StopAttack();
	void StartAim();
	void StopAim();

private:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
};
