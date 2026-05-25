#include "EscapingPoint.h"
#include "Components/BoxComponent.h"
#include "Components/PointLightComponent.h"
#include "GameMode/MainGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "UI/MainHUD.h"
#include "Ai/Boss_PoliceZombie.h"
#include "EngineUtils.h"

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
	//플레이어 확인
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (OtherActor && OtherActor == PlayerPawn)
	{
		//탈출 대기 시간 타이머 작동 & 로그 타이머 작동
		GetWorldTimerManager().SetTimer(EscapeTimer, this, &AEscapingPoint::PlayerEscaped, EscapeDuration, false);
		GetWorldTimerManager().SetTimer(LogTimer, this, &AEscapingPoint::RunLogTimer, 0.1f, true);
		
		//탈출지점 UI 출력
		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		if (PC)
		{
			AMainHUD* HUD = Cast<AMainHUD>(PC->GetHUD());
			if (HUD)
			{
				HUD->ShowExtractionHUD();
			
			}
		}
	}
}

//플레이어 탈출 지점에서 이탈 확인
void AEscapingPoint::OnCollisionEndOverlap(
	UPrimitiveComponent* OverlappedComp, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	//플레이어인지 확인
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (OtherActor && OtherActor == PlayerPawn)
	{
		//탈출 타이머 초기화
		GetWorldTimerManager().ClearTimer(EscapeTimer);
		//탈출지점 UI 제거
		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		if (PC)
		{
			AMainHUD* HUD = Cast<AMainHUD>(PC->GetHUD());
			if (HUD)
			{
				HUD->HideExtractionHUD();
			}
		}
	}
	
}

//탈출 지점에 3초 버틸 시 호출
void AEscapingPoint::PlayerEscaped()
{

	FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	bool bIsBossStage = CurrentLevelName.Equals(TEXT("BossStage"));

	bool bIsBossDead = false;
	if (bIsBossStage)
	{
		bIsBossDead = true;
		for (TActorIterator<ABoss_PoliceZombie> It(GetWorld()); It; ++It)
		{
			if (It->GetCurrentState() != EZombieState::Dead)
			{
				bIsBossDead = false;
				break;
			}
		}
	}

	if (bIsBossStage && bIsBossDead)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		if (PC)
		{
			APawn* PlayerPawn = PC->GetPawn();
			if (PlayerPawn)
			{
				PlayerPawn->DisableInput(PC);
			}

			AMainHUD* HUD = Cast<AMainHUD>(PC->GetHUD());
			if (HUD)
			{
				HUD->ShowHappyEndingHUD();

				FInputModeUIOnly InputMode;

				PC->SetInputMode(InputMode);
				PC->bShowMouseCursor = true; // 필요 시 커서 표시 활성화

				return;
			}
		}
	}

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

float AEscapingPoint::GetRemainingEscapingTime() const
{
	return GetWorldTimerManager().GetTimerRemaining(EscapeTimer);
}

float AEscapingPoint::GetMaxEscapingDuration() const
{
	return EscapeDuration;
}
