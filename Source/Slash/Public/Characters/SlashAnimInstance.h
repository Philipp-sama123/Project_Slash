// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterTypes.h"
#include "Animation/AnimInstance.h"
#include "SlashAnimInstance.generated.h"


class ASlashCharacter;
class UCharacterMovementComponent;

UCLASS()
class SLASH_API USlashAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly)
	ASlashCharacter* SlashCharacter;

	UPROPERTY(BlueprintReadOnly, Category="Movement")
	UCharacterMovementComponent* SlashCharacterMovement;

	UPROPERTY(BlueprintReadOnly, Category="Movement")
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category="Movement")
	bool IsFalling;

	UPROPERTY(BlueprintReadOnly, Category="Movement | Character State")
	ECharacterState CharacterState;
};
