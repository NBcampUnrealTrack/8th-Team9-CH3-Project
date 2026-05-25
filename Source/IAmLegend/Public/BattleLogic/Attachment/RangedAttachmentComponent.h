// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BattleLogic/Attachment/AttachmentDataAsset.h"
#include "RangedAttachmentComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class IAMLEGEND_API URangedAttachmentComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY()
	class ARangedWeaponBase* OwnerWeapon;

	UPROPERTY()
	class URangedWeaponDataAsset* WeaponData;

	UPROPERTY()
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon|Attachment")
	TMap<EAttachmentSlot, class UAttachmentDataAsset*> CurrentAttachments;

	UPROPERTY()
	TMap<EAttachmentSlot, UStaticMeshComponent*> AttachmentMeshComponents;

public:	
	URangedAttachmentComponent();

protected:
	virtual void BeginPlay() override;

public:	
	void RefreshWeaponStats();	// 장착한 부착물에 따라 무기의 스탯을 갱신하는 함수
	bool IsCanEquipAttachment(UAttachmentDataAsset* AttachmentData) const;		// 특정 부착물을 장착할 수 있는지 확인하는 함수
	bool EquipAttachment(UAttachmentDataAsset* AttachmentData);					// 부착물을 장착하는 함수
	UAttachmentDataAsset* UnequipAttachment(EAttachmentSlot AttachmentSlot);	// 부착물을 해제하는 함수
	void UpdateAttachmentMesh(EAttachmentSlot AttachmentSlot);					// 부착물의 메쉬를 업데이트하는 함수
	TMap<EAttachmentSlot, UAttachmentDataAsset*> GetCurrentAttachments() const;	// 현재 장착된 부착물들의 맵을 반환하는 함수 (무기 해제 시 사용)
	
};
