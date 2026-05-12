#include "EscapingPoint.h"
#include "Components/BoxComponent.h"
#include "Components/PointLightComponent.h"
#include "GameMode/MainGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AEscapingPoint::AEscapingPoint()
{

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	SetRootComponent(Collision);

	//플레이어 콜리젼 접촉 시 설정
	Collision->OnComponentBeginOverlap.AddDynamic(this, &AEscapingPoint::OnCollisionOverlap);
	Collision->OnComponentEndOverlap.AddDynamic(this, &AEscapingPoint::OnCollisionEndOverlap);
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMesh->SetupAttachment(Collision);
	
	//초록색 빛 설정
	GreenLight = CreateDefaultSubobject<UPointLightComponent>("GreenLight");
	GreenLight->SetupAttachment(Collision);
	GreenLight->SetLightColor(FLinearColor(0.0f, 1.0f, 0.0f));
	
	//탈출 지점 파티클 설정
	SmokeParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SmokeParticle"));
	SmokeParticle->SetupAttachment(RootComponent);
	SmokeParticle->SetAutoActivate(true);
	
	//탈출 대기 시간 설정
	EscapeDuration = 3.0f;
}


//플레이어 오버랩 확인
void AEscapingPoint::OnCollisionOverlap(
	UPrimitiveComponent* OverlappedComp, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, 
	bool bFromSweep, 
	const FHitResult& SweepResult)
{

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	//플레이어 탈출 성공
	if (OtherActor && OtherActor == PlayerPawn)
	{
		//탈출 대기 시간 타이머 작동 & 로그 타이머 작동
		GetWorldTimerManager().SetTimer(EscapeTimer, this, &AEscapingPoint::PlayerEscaped, EscapeDuration, false);
		GetWorldTimerManager().SetTimer(LogTimer, this, &AEscapingPoint::RunLogTimer, 0.1f, true);
		
	}
}

//플레이어 탈출 지점에서 이탈 확인
void AEscapingPoint::OnCollisionEndOverlap(
	UPrimitiveComponent* OverlappedComp, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (OtherActor && OtherActor == PlayerPawn)
	{
		//탈출 타이머 초기화
		GetWorldTimerManager().ClearTimer(EscapeTimer);
		UE_LOG(LogTemp, Warning, TEXT("탈출 지점에서 벗어났습니다."));
	}
	
}

//탈출 지점에 3초 버틸 시 호출
void AEscapingPoint::PlayerEscaped()
{
	AMainGameModeBase* GameMode = Cast<AMainGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (GameMode) 
	{
		//탈출 성공
		GameMode->EndStage(true);
	}
}

//탈출 타이머 확인용 로그
void AEscapingPoint::RunLogTimer()
{
	if (GetWorldTimerManager().IsTimerActive(EscapeTimer))
	{
		//탈출까지 남은 시간 로그 출력
		float Remaining = GetWorldTimerManager().GetTimerRemaining(EscapeTimer);
		UE_LOG(LogTemp, Warning, TEXT("Remaining Time: %f"), Remaining);
	}
	else
	{
		//탈출 타이머가 초기화되면 로그 타이머도 같이 초기화
		GetWorldTimerManager().ClearTimer(LogTimer);
	}
}
