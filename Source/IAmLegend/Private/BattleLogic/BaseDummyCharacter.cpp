// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleLogic/BaseDummyCharacter.h"

// Sets default values
ABaseDummyCharacter::ABaseDummyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MaxHealth = 100.f;
	Health = MaxHealth;
	MoveSpeed = 600.f;
	JumpHeight = 600.f;
}

// Called when the game starts or when spawned
void ABaseDummyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseDummyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float ABaseDummyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{

    if (Health <= 0.f)
    {
        return 0.f;
    }

    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (ActualDamage > 0.f)
    {
        Health = FMath::Clamp(Health - ActualDamage, 0.f, MaxHealth);

        

        if (Health <= 0.f)
        {
            Die();
        }
    }

    return ActualDamage;
}

void ABaseDummyCharacter::Die()
{
	Destroy();
}

