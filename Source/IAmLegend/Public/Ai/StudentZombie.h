// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ai/PoliceZombie.h"
#include "StudentZombie.generated.h"

/**
 * 
 */
UCLASS()
class IAMLEGEND_API AStudentZombie : public APoliceZombie
{
	GENERATED_BODY()
	
	AStudentZombie();
public:
	virtual void PlayAttackMontage() override;
	virtual void ResetAttack() override;

};
