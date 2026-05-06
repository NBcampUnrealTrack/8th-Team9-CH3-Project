#include "BaseItemActor.h"
#include "GameFramework/Character.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ItemDataAsset.h"

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
	// Base는 기본 동작 없음 (override 용도)
}