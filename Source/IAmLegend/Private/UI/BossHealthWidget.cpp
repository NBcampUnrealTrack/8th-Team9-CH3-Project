// BossHealthWidget.cpp


#include "UI/BossHealthWidget.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "AI/Boss_PoliceZombie.h"
#include "Ai/BaseZombie_Ai.h"
#include "BehaviorTree/BlackboardComponent.h"

void UBossHealthWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 루트 위젯은 마우스 클릭을 막지 않으면서 Tick이 돌아가도록 설정합니다.
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	// 내부의 프로그레스 바만 콕 집어서 숨겨줍니다.
	if (BossProgressBar)
	{
		BossProgressBar->SetVisibility(ESlateVisibility::Collapsed);
	}
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

	if (!BossZombie || BossZombie->GetCurrentState() == EZombieState::Dead)
	{
		if (BossProgressBar && BossProgressBar->GetVisibility() != ESlateVisibility::Collapsed)
		{
			BossProgressBar->SetVisibility(ESlateVisibility::Collapsed);
		}
		return;
	}

	float Distance = PlayerPawn->GetDistanceTo(BossZombie);
	bool bIsCloseEnough = (Distance <= BossDetectDistance);

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

	// 가시성 조작 대상을 위젯 본체가 아닌 BossProgressBar로 변경
	if (bIsCloseEnough || bIsTargetingPlayer)
	{
		if (BossProgressBar && BossProgressBar->GetVisibility() != ESlateVisibility::Visible)
		{
			BossProgressBar->SetVisibility(ESlateVisibility::Visible);
		}

		if (BossProgressBar && BossZombie->MaxHealth > 0.f)
		{
			float HealthRatio = FMath::Clamp(BossZombie->Health / BossZombie->MaxHealth, 0.f, 1.f);
			BossProgressBar->SetPercent(HealthRatio);
		}
	}
	else
	{
		if (BossProgressBar && BossProgressBar->GetVisibility() != ESlateVisibility::Collapsed)
		{
			BossProgressBar->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}