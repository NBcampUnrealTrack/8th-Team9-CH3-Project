#include "Item/CraftingVolumeActor.h"
#include "Components/SphereComponent.h"
#include "Item/InventoryComponent.h" 
#include "Components/WidgetComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Character/HanPlayerCharacter.h" // 캐릭터 참조를 위해 필요

ACraftingVolumeActor::ACraftingVolumeActor()
{
    PrimaryActorTick.bCanEverTick = false;

    // 콜라이더 설정
    InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
    RootComponent = InteractionSphere;
    InteractionSphere->SetSphereRadius(150.f);
    InteractionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

    // 메쉬 설정
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);

	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	InteractionWidget->SetupAttachment(RootComponent);
	InteractionWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InteractionWidget->SetDrawAtDesiredSize(true);
	InteractionWidget->SetVisibility(false);

 
}

void ACraftingVolumeActor::BeginPlay()
{
    Super::BeginPlay();

    // 오버랩 이벤트 바인딩
    InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &ACraftingVolumeActor::OnOverlapBegin);
    InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &ACraftingVolumeActor::OnOverlapEnd);
}

void ACraftingVolumeActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AHanPlayerCharacter* Player = Cast<AHanPlayerCharacter>(OtherActor))
	{
		// 위젯 띄우기 
		if (InteractionWidget) InteractionWidget->SetVisibility(true);
        
		// 인벤토리 컴포넌트를 찾아서 제작대 UI 실행
		if (UInventoryComponent* InvComp = Player->FindComponentByClass<UInventoryComponent>())
		{
			// true를 전달하여 제작창을 활성화
			InvComp->ToggleCraftingUI(true);
			UE_LOG(LogTemp, Log, TEXT("제작대 접촉: 제작 UI 자동 활성화"));
		}
	}
}

void ACraftingVolumeActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AHanPlayerCharacter* Player = Cast<AHanPlayerCharacter>(OtherActor))
	{
		// 위젯 숨기기
		if (InteractionWidget) InteractionWidget->SetVisibility(false);
        
		// 제작대를 벗어나면 제작 UI 자동으로 닫기
		if (UInventoryComponent* InvComp = Player->FindComponentByClass<UInventoryComponent>())
		{
			// false를 전달하여 제작창을 비활성화
			InvComp->ToggleCraftingUI(false);
			UE_LOG(LogTemp, Log, TEXT("제작대 이탈: 제작 UI 자동 비활성화"));
		}
	}
}

void ACraftingVolumeActor::OnInteract(AActor* Interactor)
{
    if (!Interactor) return;

    if (UInventoryComponent* InvComp = Interactor->FindComponentByClass<UInventoryComponent>())
    {
       InvComp->ToggleCraftingUI(true);
       UE_LOG(LogTemp, Log, TEXT("제작대 상호작용: 제작 UI 호출"));
    }
}