#include "Gamemode/MainGameModeBase.h"
#include "UI/MainHUD.h"
#include "Spawn/SpawnManager.h"
#include "Character/HanPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Gamemode/MainGameInstance.h"
#include "Gamemode/MainGameStateBase.h"
#include "Item/ItemDataAsset.h"

AMainGameModeBase::AMainGameModeBase()
{
	//게임모드 기본 클래스 세팅
	GameStateClass = AMainGameStateBase::StaticClass();
	DefaultPawnClass = AHanPlayerCharacter::StaticClass();
	
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
	
	//레벨 불러오기
	LoadStageLevel(EStageType::Shelter);
}


// 스테이지 입장 시 초기 설정, 어떤 스테이지 입장했는지 Enum 값을 받음 
void AMainGameModeBase::EnterStage(EStageType StageType)
{
	//보스 스테이지 진입 시도 시 백신 아이템 존재 확인
	if (StageType == EStageType::Boss)
	{
		UMainGameInstance* GI = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		if (!GI) return;
		
		//인벤토리에 백신 보유 여부 확인
		bool bPossessVaccine = false;
		for (const FItemSlot& ItemSlot : GI->GetItemsFromGlobalInventory())
		{
			if (!ItemSlot.ItemData) continue;
			
			if (ItemSlot.ItemData->ItemName=="백신")
			{
				bPossessVaccine = true;
			}
		}
		//백신 보유 하지 않으면 경고 메시지 이후 맵 이동 X
		if (bPossessVaccine == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("백신을 보유하고 있지 않습니다."))
			return;
		}
	}
	

	
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		// 입력 모드를 게임 전용으로 변경
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(false);
	}
	
	//스테이지 시작 게임 인스턴스에 저장
	UMainGameInstance* GI = Cast<UMainGameInstance>(GetGameInstance());
	if (!GI) return;
	
	GI->SetbIsStageStarted(true);
	
	
	//현재 스테이지 저장 및 스테이지 레벨 열기
	LoadStageLevel(StageType);	
}

//스테이지 시작
void AMainGameModeBase::StartStage()
{
	//타이머를 통해 스테이지 제한 시간 설정
	GetWorldTimerManager().SetTimer(StageTimer, this, &AMainGameModeBase::OnStageTimeUp, MaxStageDuration, false);
	
	//스테이지 UI 출력
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return;
	
	AMainHUD* HUD = Cast<AMainHUD>(PC->GetHUD());
	if (HUD)
	{
		HUD->ShowStageHUD();
	}
	
	//플레이어 탈출 여부 실패로 초기 설정
	UMainGameInstance* GI = Cast<UMainGameInstance>(GetGameInstance());
	if (!GI) return;
	GI->SetbIsPlayerEscaped(false);
	
	//스폰 매니저에서 적 스폰
	ASpawnManager* SpawnManager = Cast<ASpawnManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpawnManager::StaticClass()));
	if (SpawnManager)
	{
		//스테이지 제한 시간 종료 여부 확인
		SpawnManager->SpawnEnemyAtStage(false);
	}
}

//스테이지 제한 시간 종료(몬스터 대량 스폰)
void AMainGameModeBase::OnStageTimeUp()
{
	//스폰 매니저에서 적 대량스폰
	ASpawnManager* SpawnManager = Cast<ASpawnManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ASpawnManager::StaticClass()));
	if (SpawnManager)
	{
		SpawnManager->SpawnEnemyAtStage(true);
	}
}


//스테이지 종료, 탈출 성공 실패를 확인
void AMainGameModeBase::EndStage(bool bIsPlayerEscaped)
{	
	//스테이지 UI 숨김
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return;
	AMainHUD* HUD = Cast<AMainHUD>(PC->GetHUD());
	if (HUD)
	{
		HUD->HideStageHUD();
	}
	
	//플레이어 탈출 성공 판정
	if (bIsPlayerEscaped)
	{
		SuccessEscape();
		
	}
	else
	{
		FailEscape();
	}
	GetWorldTimerManager().ClearTimer(StageTimer);
	
	//현재 스테이지 쉘터로 변경
	UMainGameInstance* GI = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (!GI) return;
	GI->SetbIsStageStarted(false);
	GI->SetCurrentStage(EStageType::Shelter);
}
//스테이지 남은 시간 가져가기
float AMainGameModeBase::GetRemainingStageTime() const
{
	return GetWorldTimerManager().GetTimerRemaining(StageTimer);
}

//적 처치 시 점수 저장
void AMainGameModeBase::killedEnemy()
{
	++PlayerKillCount;
}

//적 처치 수 가져가기
int32 AMainGameModeBase::GetPlayerKillCount() const
{
	return PlayerKillCount;
}

//탈출 지점을 통해 탈출 성공 
void AMainGameModeBase::SuccessEscape()
{
	UMainGameInstance* GI = Cast<UMainGameInstance>(GetGameInstance());
	if (!GI) return;
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) return;
	
	//게임 일시정지
	UGameplayStatics::SetGamePaused(GetWorld(), true);
	PC->bShowMouseCursor = true;
	PC->SetInputMode(FInputModeGameAndUI());
	
	UE_LOG(LogTemp, Warning, TEXT("Player Escaped"));
	//플레이어 탈출 성공 설정
	GI->SetbIsPlayerEscaped(true);
	
	//정산 UI 출력

	AMainHUD* HUD = Cast<AMainHUD>(PC->GetHUD());
	if (HUD)
	{
		HUD->HideExtractionHUD();
		HUD->ShowStageResultHUD();
	}
}


//탈출 실패 시 게임 오버 UI 출력
void AMainGameModeBase::FailEscape()
{
	//게임 오버 UI 출력
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		AMainHUD* HUD = Cast<AMainHUD>(PC->GetHUD());
		if (HUD)
		{
			HUD->ShowGameOverHUD();
		}
	}
}

//스테이지 레벨 열기
void AMainGameModeBase::LoadStageLevel(EStageType StageType)
{
	//게임 인스턴스에 현재 스테이지 저장
	UMainGameInstance* GI = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (!GI) return;
	GI->SetCurrentStage(StageType);
	

	//TMap에서 키를 통해 현재 맵의 이름 가져오기
	const FName* LevelName = LevelMapNames.Find(StageType);
	if (!LevelName) return;
	UGameplayStatics::OpenLevel(GetWorld(), *LevelName);

}

//스테이지 제한 시간 전달
float AMainGameModeBase::GetMaxStageDuration() const
{
	return MaxStageDuration;
}
