#include "Gamemode/MainGameModeBase.h"

#include "MainHUD.h"
#include "Spawn/SpawnManager.h"
#include "Character/HanPlayerCharacter.h"
#include "Gamemode/MainGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Gamemode/MainGameInstance.h"

AMainGameModeBase::AMainGameModeBase()
{
	//게임모드 기본 클래스 세팅
	GameStateClass = AMainGameStateBase::StaticClass();
	DefaultPawnClass = AHanPlayerCharacter::StaticClass();
	HUDClass = AMainHUD::StaticClass();
	
	//초기값 세팅
	MaxStageDuration = 5.0f;
	PlayerKillCount = 0;
	

}

void AMainGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
	UMainGameInstance* GI = Cast<UMainGameInstance>(GetGameInstance());
	if (!GI) return;

	
	//스테이지 시작했는지 확인
	if (GI->GetbIsStageStarted())
	{
		StartStage();
	}
	
	
	UE_LOG(LogTemp, Warning, TEXT("Is Player Escape: %s"), GI->GetbIsPlayerEscaped() ? TEXT("true") : TEXT("false"));
}


//게임 시작(쉘터 진입)
void AMainGameModeBase::StartGame()
{
	UMainGameInstance* GI = Cast<UMainGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->SetbIsGameStarted(true);
	}
	
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		// 입력 모드를 게임 전용으로 변경
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
	}
	
	UGameplayStatics::OpenLevel(GetWorld(), FName("Shelter"));
}


// 스테이지 입장 시 초기 설정, 어떤 스테이지 입장했는지 인덱스 값을 받음 
void AMainGameModeBase::EnterStage(int32 StageIndex)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		// 입력 모드를 게임 전용으로 변경
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
	}
	
	UMainGameInstance* GI = Cast<UMainGameInstance>(GetGameInstance());
	if (!GI) return;
	
	GI->SetbIsStageStarted(true);
	
	UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[StageIndex]);
	
}

//스테이지 시작
void AMainGameModeBase::StartStage()
{
	//타이머를 통해 제한 시간 설정
	GetWorldTimerManager().SetTimer(StageTimer, this, &AMainGameModeBase::OnStageTimeUp, MaxStageDuration, false);
	UE_LOG(LogTemp, Warning, TEXT("Timer Start"));
	
	//스테이지 UI 출력
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		AMainHUD* HUD = Cast<AMainHUD>(PC->GetHUD());
		if (HUD)
		{
			HUD->ShowStageHUD();
		}
	}
	
	
	//플레이어 탈출 여부 실패로 초기 설정
	UMainGameInstance* GI = Cast<UMainGameInstance>(GetGameInstance());
	if (!GI) return;
	GI->SetbIsPlayerEscaped(false);
	
	//스폰 매니저에서 적 스폰
	ASpawnManager* SpawnManager = Cast<ASpawnManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpawnManager::StaticClass()));
	if (SpawnManager)
	{
		SpawnManager->SpawnEnemyAtStage();
	}
}

//스테이지 제한 시간 종료
void AMainGameModeBase::OnStageTimeUp()
{
	
	EndStage(false);
	UE_LOG(LogTemp, Warning, TEXT("Stage End"));
}


//스테이지 종료, 탈출 성공 실패를 확인
void AMainGameModeBase::EndStage(bool bIsPlayerEscaped)
{
	UMainGameInstance* GI = Cast<UMainGameInstance>(GetGameInstance());
	if (!GI) return;
	
	//플레이어 탈출 성공 판정
	if (bIsPlayerEscaped)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Escaped"));
		GI->SetbIsPlayerEscaped(true);
		
		GetWorldTimerManager().ClearTimer(StageTimer);
		
	}
	
	
	//스테이지 종료
	GI->SetbIsStageStarted(false);
	UGameplayStatics::OpenLevel(GetWorld(), FName("Shelter"));
}

float AMainGameModeBase::GetRemainingStageTime() const
{
	return GetWorldTimerManager().GetTimerRemaining(StageTimer);
}

//적 처치 시 점수 저장
void AMainGameModeBase::killedEnemy()
{
	++PlayerKillCount;
}

int32 AMainGameModeBase::GetPlayerKillCount() const
{
	return PlayerKillCount;
}
