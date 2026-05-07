#include "Item/BaseItemActor.h"
#include "GameFramework/Character.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Item/ItemDataAsset.h"
#include "Components/WidgetComponent.h"
#include "Item/InventoryComponent.h"

ABaseItemActor::ABaseItemActor()
{
	//콜라이더와 메쉬 설정
	Collider = CreateDefaultSubobject<USphereComponent>("Collider");
	RootComponent = Collider;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(Collider);

	// InteractionWidget 생성
	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>("InteractionWidget");
	InteractionWidget->SetupAttachment(RootComponent); // 루트에 붙임
    
	// UI 위치 설정 (아이템 살짝 위)
	InteractionWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f)); 
	
	
	//UI 모니터 정면을 향해 그려진다
	InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InteractionWidget->SetDrawAtDesiredSize(true);
	
	// 처음엔 안 보이게 설정
	InteractionWidget->SetVisibility(false);

	// 충돌 설정 및 바인딩
	Collider->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Collider->SetGenerateOverlapEvents(true);

	Collider->OnComponentBeginOverlap.AddDynamic(this, &ABaseItemActor::OnOverlap);
	// EndOverlap도 연결해줘야 나갈 때 꺼집니다!
	Collider->OnComponentEndOverlap.AddDynamic(this, &ABaseItemActor::OnEndOverlap); 
}

void ABaseItemActor::BeginPlay()
{
	Super::BeginPlay();
	
	InitFromData();
}

void ABaseItemActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	
	InitFromData();
}

void ABaseItemActor::InitFromData()
{
	if (!ItemData) return;

	if (Mesh && ItemData->ItemMesh)
	{
		Mesh->SetStaticMesh(ItemData->ItemMesh);
	}
}

void ABaseItemActor::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	ACharacter* Player = Cast<ACharacter>(OtherActor);
	if (Player)
	{
		
		OnPlayerEntered(Player);
	}
}

void ABaseItemActor::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ACharacter* Player = Cast<ACharacter>(OtherActor);
	if (Player)
	{
		// 여기서 위젯 Visible(false) 처리됨
		OnPlayerExited(Player);
	}
}

void ABaseItemActor::ApplyPickup(ACharacter* Player)
{
	if (!Player || !ItemData) return;

	//캐릭터로부터 인벤토리 컴포넌트를 가져오기
	UInventoryComponent* InvComp = Player->FindComponentByClass<UInventoryComponent>();

	if (InvComp)
	{
		// 인벤토리에 이 액터가 들고 있는 ItemData를 추가
		bool bAdded = InvComp->AddItem(ItemData);

		if (bAdded)
		{
			
			UE_LOG(LogTemp, Log, TEXT("%s 아이템 획득 성공!"), *ItemData->ItemName);
			Destroy();
		}
		else 
		{
			
			UE_LOG(LogTemp, Warning, TEXT("인벤토리가 가득 차서 %s를 주울 수 없습니다."), *ItemData->ItemName);
		}
	}
}

void ABaseItemActor::Interact(ACharacter* Player)
{
	//상호작용 후 하는 행동 
	ApplyPickup(Player);
}

FString ABaseItemActor::InteractionText() const
{
	if (ItemData)
	{
		return FString::Printf(TEXT("%s 획득 (F)"), *ItemData->ItemName);
	}
	return TEXT(" 상호작용 (F)");
}

// 플레이어가 들어왔을 때
void ABaseItemActor::OnPlayerEntered(ACharacter* Player)
{
	if (InteractionWidget)
	{
		InteractionWidget->SetVisibility(true);
		
	}
}

// 플레이어가 나갔을 때
void ABaseItemActor::OnPlayerExited(ACharacter* Player)
{
	if (InteractionWidget)
	{
		InteractionWidget->SetVisibility(false);
	}
}
