// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"

#include "Enemy.generated.h"

UCLASS()
class SLASH_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetHit(const FVector& ImpactPoint) override;
	void DirectionalHitReact(const FVector& ImpactPoint);

private:
	/**
	 * Animation Montages
	 */
	UPROPERTY(EditDefaultsOnly, Category="Montages")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category="Sounds")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category="Visual Effects")
	UParticleSystem* HitParticles;

protected:
	virtual void BeginPlay() override;
	/*
	* Animation Montage Functions
	*/
	void PlayHitReactMontage(const FName& SectionName);
};
