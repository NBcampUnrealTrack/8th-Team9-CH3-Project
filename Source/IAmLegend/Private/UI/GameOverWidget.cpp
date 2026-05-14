// GameOverWidget.cpp


#include "UI/GameOverWidget.h"

#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Gamemode/MainGameModeBase.h"
#include "Gamemode/MainGameStateBase.h"
#include "Gamemode/MainGameInstance.h"

void UGameOverWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 타이틀 버튼 연결
	if (TitleButton)
	{
		TitleButton->OnClicked.AddDynamic(this, &UGameOverWidget::OnTitleButtonClicked);
	}

	// 게임 모드에서 점수(MainGameStateBase)와 킬 수(MainGameModeBase) 가져오기
	AMainGameStateBase* GS = Cast<AMainGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
	AMainGameModeBase* GM = Cast<AMainGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	// 점수 UI 업데이트
	if (TotalScore && GS)
	{
		FString ScoreString = FString::Printf(TEXT("총 점수 : %d"), GS->KillScore);
		TotalScore->SetText(FText::FromString(ScoreString));
	}

	// 좀비 Kill 수 UI 업데이트
	if (KillCount && GM)
	{
		FString KillString = FString::Printf(TEXT("처치한 좀비 : %d"), GM->GetPlayerKillCount());
		KillCount->SetText(FText::FromString(KillString));
	}
}

void UGameOverWidget::OnTitleButtonClicked()
{
	// 일시 정지 해제
	UGameplayStatics::SetGamePaused(GetWorld(), false);

	// GameInstance 상태 초기화
	UMainGameInstance* GI = Cast<UMainGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->SetbIsGameStarted(false);
		GI->SetbIsStageStarted(false);
	}

	// 타이틀 화면으로 이동
	UGameplayStatics::OpenLevel(GetWorld(), FName("MainTitleLevel"));
}