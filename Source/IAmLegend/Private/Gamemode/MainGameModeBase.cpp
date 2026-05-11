#include "Gamemode/MainGameModeBase.h"

#include "EnemySpawnVolume.h"
#include "Character/HanPlayerCharacter.h"
#include "Gamemode/MainGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "TestUserWidget.h"
#include "Gamemode/MainGameInstance.h"

AMainGameModeBase::AMainGameModeBase()
{
	//게임모드 기본 클래스 세팅
	GameStateClass = AMainGameStateBase::StaticClass();
	DefaultPawnClass = AHanPlayerCharacter::StaticClass();
	
	//초기값 세팅
	MaxStageDuration = 5.0f;
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
	
	//플레이어 탈출 여부 실패로 초기 설정
	UMainGameInstance* GI = Cast<UMainGameInstance>(GetGameInstance());
	if (!GI) return;
	GI->SetbIsPlayerEscaped(false);
	
	SpawnEnemyAtStage();
}

void AMainGameModeBase::SpawnEnemyAtStage()
{
	//적 스폰
	TArray<AActor*> EnemySpawnVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawnVolume::StaticClass(), EnemySpawnVolumes);
	if (EnemySpawnVolumes.Num() <= 0) return;
	for (int32 i=0; i<EnemySpawnVolumes.Num(); ++i)
	{
		AEnemySpawnVolume* EnemySpawnVolume = Cast<AEnemySpawnVolume>(EnemySpawnVolumes[i]);
		EnemySpawnVolume->TrySpawn();
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