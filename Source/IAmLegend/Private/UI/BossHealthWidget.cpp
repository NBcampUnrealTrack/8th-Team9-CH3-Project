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

	// UI 숨기기
	SetHealthUIVisibility(ESlateVisibility::Collapsed);
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

	// 보스가 없거나 죽으면 숨김
	if (!BossZombie || BossZombie->GetCurrentState() == EZombieState::Dead)
	{
		SetHealthUIVisibility(ESlateVisibility::Collapsed);
		return;
	}

	bool bIsTargetingPlayer = false;
	ABaseZombie_Ai* BossAI = Cast<ABaseZombie_Ai>(BossZombie->GetController());
	if (BossAI && BossAI->GetBlackboardComponent())
	{
		UObject* TargetObject = BossAI->GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor"));
		if (TargetObject != nullptr)
		{
			bIsTargetingPlayer = true;
		}
	}

	// 보스 근처에 가거나 타겟팅 되었을 때 표시
	if (bIsTargetingPlayer)
	{
		SetHealthUIVisibility(ESlateVisibility::Visible);

		if (BossProgressBar && BossZombie->MaxHealth > 0.f)
		{
			float HealthRatio = FMath::Clamp(BossZombie->Health / BossZombie->MaxHealth, 0.f, 1.f);
			BossProgressBar->SetPercent(HealthRatio);
		}
	}
	// 다시 멀어지면 숨김
	else
	{
		SetHealthUIVisibility(ESlateVisibility::Collapsed);
	}
}