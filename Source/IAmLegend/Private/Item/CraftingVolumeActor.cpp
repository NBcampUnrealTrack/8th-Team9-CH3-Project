#include "Item/CraftingVolumeActor.h"
#include "Components/SphereComponent.h"
#include "Item/InventoryComponent.h" 
#include "Components/WidgetComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Character/HanPlayerCharacter.h"

ACraftingVolumeActor::ACraftingVolumeActor()
{
    PrimaryActorTick.bCanEverTick = false;

    // 콜라이더 및 메쉬 설정
    InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
    RootComponent = InteractionSphere;
    InteractionSphere->SetSphereRadius(150.f);
    InteractionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);

    // 위젯 설정
    InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
    InteractionWidget->SetupAttachment(RootComponent);
    InteractionWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 20.0f));
    InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);
    InteractionWidget->SetDrawAtDesiredSize(true);
    InteractionWidget->SetVisibility(false);

    // 아이템과 동일한 UI 위젯 에셋 바인딩
    static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("/Game/Item/WBP_interaction.WBP_interaction_C"));
    if (WidgetClassFinder.Succeeded())
    {
        InteractionWidget->SetWidgetClass(WidgetClassFinder.Class);
    }
}

void ACraftingVolumeActor::BeginPlay()
{
    Super::BeginPlay();

    InteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &ACraftingVolumeActor::OnOverlapBegin);
    InteractionSphere->OnComponentEndOverlap.AddDynamic(this, &ACraftingVolumeActor::OnOverlapEnd);
}

void ACraftingVolumeActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (ACharacter* Player = Cast<ACharacter>(OtherActor))
    {
        // 인터페이스 기능 호출
        OnPlayerEntered(Player);
    }
}

void ACraftingVolumeActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (ACharacter* Player = Cast<ACharacter>(OtherActor))
    {
        // 인터페이스 기능 호출
        OnPlayerExited(Player);
    }
}

// 기존 캐릭터가 호출하던 OnInteract를 인터페이스 함수인 Interact로 토스해줍니다.
void ACraftingVolumeActor::OnInteract(AActor* Interactor)
{
    if (ACharacter* Player = Cast<ACharacter>(Interactor))
    {
        Interact(Player);
    }
}

// --- 인터페이스 실제 구현부 ---

// F키를 눌렀을 때 실행될 제작대 본연의 기능
void ACraftingVolumeActor::Interact(ACharacter* Player)
{
    if (!Player) return;

    if (UInventoryComponent* InvComp = Player->FindComponentByClass<UInventoryComponent>())
    {
        InvComp->ToggleCraftingUI(true);
        UE_LOG(LogTemp, Log, TEXT("제작대 상호작용 인터페이스 호출 성공: 제작 UI 활성화"));
    }
}

FString ACraftingVolumeActor::InteractionText() const
{
    return TEXT("제작대 열기 (F)");
}

void ACraftingVolumeActor::OnPlayerEntered(ACharacter* Player)
{
    if (InteractionWidget)
    {
        InteractionWidget->SetVisibility(true);
    }

    if (AHanPlayerCharacter* HanChar = Cast<AHanPlayerCharacter>(Player))
    {
        HanChar->SetTargetItem(this);
        UE_LOG(LogTemp, Log, TEXT("인터페이스 - 제작대 타겟 설정 완료"));
    }
}

void ACraftingVolumeActor::OnPlayerExited(ACharacter* Player)
{
    if (InteractionWidget)
    {
        InteractionWidget->SetVisibility(false);
    }

    if (AHanPlayerCharacter* HanChar = Cast<AHanPlayerCharacter>(Player))
    {
        HanChar->SetTargetItem(nullptr);
        UE_LOG(LogTemp, Log, TEXT("인터페이스 - 제작대 타겟 해제"));

        // 멀어지면 UI 닫기
        if (UInventoryComponent* InvComp = HanChar->FindComponentByClass<UInventoryComponent>())
        {
            InvComp->ToggleCraftingUI(false);
        }
    }
}