#include "BaseItemActor.h"
#include "GameFramework/Character.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ItemDataAsset.h"
#include "InventoryComponent.h"

ABaseItemActor::ABaseItemActor()
{

	Collider = CreateDefaultSubobject<USphereComponent>("Collider");
	RootComponent = Collider;

	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(Collider);

	Collider->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Collider->SetGenerateOverlapEvents(true);

	Collider->OnComponentBeginOverlap.AddDynamic(this, &ABaseItemActor::OnOverlap);
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

void ABaseItemActor::OnOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	ACharacter* Player = Cast<ACharacter>(OtherActor);
	if (!Player) return;

	if (!ItemData) return;

	ApplyPickup(Player);

	// 공통 제거 처리
	Destroy();
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