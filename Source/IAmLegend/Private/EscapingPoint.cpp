#include "EscapingPoint.h"
#include "Components/BoxComponent.h"
#include "GameMode/MainGameModeBase.h"
#include "Kismet/GameplayStatics.h"

AEscapingPoint::AEscapingPoint()
{
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Collision->SetupAttachment(Scene);

	//플레이어 콜리젼 접촉 시 설정
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AEscapingPoint::OnCollisionOverlap);
}


void AEscapingPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

//플레이어 오버랩 태그를 통해서 확인
void AEscapingPoint::OnCollisionOverlap(
	UPrimitiveComponent* OverlappedComp, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, 
	bool bFromSweep, 
	const FHitResult& SweepResult)
{
	//플레이어 탈출 성공
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		AMainGameModeBase* GameMode = Cast<AMainGameModeBase>(UGameplayStatics::GetGameMode(this));
		if (GameMode) 
		{
			GameMode->EndStage(true);
		}
	}
}
