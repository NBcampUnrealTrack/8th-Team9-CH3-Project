#include "Gamemode/MainGameModeBase.h"
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
	
	if (!GI->GetGameStarted())
	{
		SpawnTitleUI();
	}
	
	
}

void AMainGameModeBase::SpawnTitleUI()
{
	if (!TitleUIWidgetClass) return;

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	UUserWidget* UIWidget = CreateWidget<UUserWidget>(PC, TitleUIWidgetClass); 
	UMainGameInstance* GI = Cast<UMainGameInstance>(GetGameInstance());
	if (UIWidget && GI)
	{
		UIWidget->AddToViewport();
		PC->bShowMouseCursor = GI->GetUIPopUp();
	}
}

void AMainGameModeBase::SpawnStageSelectUI()
{
	if (!StageSelectUIWidgetClass) return;
	
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;
	
	UUserWidget* UIWidget = CreateWidget<UUserWidget>(PC, StageSelectUIWidgetClass); 
	UMainGameInstance* GI = Cast<UMainGameInstance>(GetGameInstance());
	if (UIWidget && GI)
	{
		UIWidget->AddToViewport();
		PC->bShowMouseCursor = GI->GetUIPopUp();
	}
}

void AMainGameModeBase::StartGame()
{
	UMainGameInstance* GI = Cast<UMainGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->SetGameStarted(true);
		GI->SetUIPopUp(false);
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

void AMainGameModeBase::EnterStageSelectZone()
{
	UMainGameInstance* GI = Cast<UMainGameInstance>(GetGameInstance());
	APlayerController* PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());
	if (GI && PC)
	{
		GI->SetUIPopUp(true);
		PC->bShowMouseCursor = GI->GetUIPopUp();
	}
}

void AMainGameModeBase::EnterStage(int32 StageIndex)
{
	UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[StageIndex]);
	StartStage();
	
}

void AMainGameModeBase::StartStage()
{
	GetWorldTimerManager().SetTimer(StageTimerHandle, this, &AMainGameModeBase::OnStageTimeUp, MaxStageDuration, false);
	UE_LOG(LogTemp, Warning, TEXT("Timer Start"));
}

void AMainGameModeBase::OnStageTimeUp()
{
	EndStage();
	UE_LOG(LogTemp, Warning, TEXT("Stage End"));
}

void AMainGameModeBase::EndStage()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName("Shelter"));
}