// BossHealthWidget.cpp


#include "UI/BossHealthWidget.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "AI/Boss_PoliceZombie.h"
#include "Ai/BaseZombie_Ai.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

// UI 가시성 일괄 변경 함수
void UBossHealthWidget::SetHealthUIVisibility(ESlateVisibility InVisibility)
{
	if (BossProgressBar) BossProgressBar->SetVisibility(InVisibility);
	if (BossFrameImage) BossFrameImage->SetVisibility(InVisibility);
	if (BossNameText) BossNameText->SetVisibility(InVisibility);
}

void UBossHealthWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	// 시작 시 UI 숨기기 및 조우 상태 초기화
	SetHealthUIVisibility(ESlateVisibility::Collapsed);
	bHasEncountered = false;
}

void UBossHealthWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn) return;

	ABoss_PoliceZombie* BossZombie = nullptr;
	for (TActorIterator<ABoss_PoliceZombie> It(GetWorld()); It; ++It)
	{
		BossZombie = *It;
		break;
	}

	// 1. 보스가 없거나, 죽었거나, [아직 프롤로그(시체 파먹기) 중]이라면 체력바를 무조건 숨김
	if (!BossZombie || BossZombie->GetCurrentState() == EZombieState::Dead || !BossZombie->bPrologueDone)
	{
		SetHealthUIVisibility(ESlateVisibility::Collapsed);
		return;
	}

	// 2. 프롤로그가 끝나고 전투가 시작되었다면 체력바 표시 및 갱신
	SetHealthUIVisibility(ESlateVisibility::Visible);

	if (BossProgressBar && BossZombie->MaxHealth > 0.f)
	{
		float HealthRatio = FMath::Clamp(BossZombie->Health / BossZombie->MaxHealth, 0.f, 1.f);
		BossProgressBar->SetPercent(HealthRatio);
	}
}