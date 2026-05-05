#pragma once

#include "CoreMinimal.h"
#include "ItemDataAsset.h"
#include "UseItemDataAsset.generated.h"


UENUM(BlueprintType)
enum class EItemType : uint8
{
	Heal,
	Damage
};

UCLASS()
class IAMLEGEND_API UUseItemDataAsset : public UItemDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	EItemType EffectType;

	UPROPERTY(EditAnywhere)
	float Value = 0.f;
	
};
