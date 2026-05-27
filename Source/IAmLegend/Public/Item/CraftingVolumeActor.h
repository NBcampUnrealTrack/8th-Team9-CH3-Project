#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableInterface.h"
#include "CraftingVolumeActor.generated.h"

UCLASS()
class IAMLEGEND_API ACraftingVolumeActor : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
    
public: 
	ACraftingVolumeActor();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<class USphereComponent> InteractionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UWidgetComponent> InteractionWidget;

public:
	// 상호작용 함수 (이것도 아마 F키로 상호작용하게 할듯)
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void OnInteract(AActor* Interactor);

	// 오버랩 함수
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	
	// --- 상호작용 인터페이스 순수 가상 함수 오버라이드 ---
	virtual void Interact(ACharacter* Player) override;
	virtual FString InteractionText() const override;
	virtual void OnPlayerEntered(ACharacter* Player) override;
	virtual void OnPlayerExited(ACharacter* Player) override;
};