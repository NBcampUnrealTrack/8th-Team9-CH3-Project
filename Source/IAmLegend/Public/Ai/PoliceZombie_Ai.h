// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Ai/BaseZombie_Ai.h"
#include "PoliceZombie_ai.generated.h"

/**
 * 
 */
UCLASS()
class IAMLEGEND_API APoliceZombie_ai : public ABaseZombie_Ai
{
	GENERATED_BODY()
	
public:
	APoliceZombie_ai(); // 생성자: AI가 태어날 때 기본 설정(시야 등)을 세팅합니다.

protected:


public:


};
