// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Items/Weapons/Weapon.h"
#include "AnimNS_AttackTrigger.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API UAnimNS_AttackTrigger : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
	                        const FAnimNotifyEventReference& EventReference) override;
	// ToDo: Maybe make the Ignore Actor Logic here and trigger it on notify end (!)
};
