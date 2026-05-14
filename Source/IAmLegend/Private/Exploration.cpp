// Exploration.cpp
// 요약: 문에 다가가면 오버랩으로 상호작용 가능한 버튼이 생성되어 'F키'를 누르면 StageChoiceWidget으로 넘어감


#include "Exploration.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Character/HanPlayerCharacter.h"

// Sets default values
AExploration::AExploration()
{
	PrimaryActorTick.bCanEverTick = false;

	// 컴포넌트 생성 및 계층 구조 설정
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	RootComponent = BoxComp;

	// [추가] 캐릭터와의 오버랩을 정상 감지하도록 충돌 활성화
	BoxComp->SetGenerateOverlapEvents(true);
	BoxComp->SetCollisionProfileName(TEXT("Trigger"));

	InteractWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidgetComp"));
	InteractWidgetComp->SetupAttachment(RootComponent);
	InteractWidgetComp->SetWidgetSpace(EWidgetSpace::World);
	InteractWidgetComp->SetVisibility(false);
}

void AExploration::BeginPlay()
{
	Super::BeginPlay();

	// 오버랩 이벤트 바인딩
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AExploration::OnBoxBeginOverlap);
	BoxComp->OnComponentEndOverlap.AddDynamic(this, &AExploration::OnBoxEndOverlap);
}

void AExploration::EnableInput(APlayerController* PlayerController)
{
	Super::EnableInput(PlayerController);

	// F키 바인딩
	if (InputComponent)
	{
		InputComponent->BindKey(EKeys::F, IE_Pressed, this, &AExploration::OnInteractKeyPressed);
	}
}

void AExploration::DisableInput(APlayerController* PlayerController)
{
	Super::DisableInput(PlayerController);
}

void AExploration::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);

	// 오버랩된 액터가 플레이어인지 확인 (캐스팅 대신 GetPlayerPawn 비교 사용)
	if (OtherActor && OtherActor == PlayerPawn)
	{
		InteractWidgetComp->SetVisibility(true);

		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		if (PC)
		{
			EnableInput(PC);
		}
	}
}

void AExploration::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);

	if (OtherActor && OtherActor == PlayerPawn)
	{
		InteractWidgetComp->SetVisibility(false);

		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		if (PC)
		{
			DisableInput(PC);
		}
	}
}

void AExploration::OnInteractKeyPressed()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);

	if (PC && TargetWidgetClass)
	{
		UUserWidget* StageChoiceWidget = CreateWidget<UUserWidget>(PC, TargetWidgetClass);
		if (StageChoiceWidget)
		{
			StageChoiceWidget->AddToViewport();

			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(StageChoiceWidget->TakeWidget());
			PC->SetInputMode(InputMode);
			PC->SetShowMouseCursor(true);
		}
	}
}