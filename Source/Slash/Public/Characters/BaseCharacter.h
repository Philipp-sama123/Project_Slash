// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UAttributeComponent;
class UAnimMontage;


UCLASS()
class SLASH_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	virtual void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

protected:
	virtual void BeginPlay() override;
	virtual void Die();
	virtual bool CanAttack();
	bool IsAlive();
	/**
	 * Components
	 */
	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

	UPROPERTY(VisibleAnywhere, Category=Weapon)
	AWeapon* EquippedWeapon;

	/**
	 * Animation Montage Functions
	 */
	virtual void PlayAttackMontage(UAnimMontage* CurrentAttackMontage);
	virtual void HandleDamage(float DamageAmount);

	void PlayHitReactMontage(const FName& SectionName);
	void DirectionalHitReact(const FVector& ImpactPoint);
	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitParticles(const FVector& ImpactPoint);


	/**
	 * Animation Montages
	*/
	UPROPERTY(EditDefaultsOnly, Category="Montages")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category="Montages")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category="Montages")
	UAnimMontage* AttackMontageSpear;

	UPROPERTY(EditDefaultsOnly, Category="Montages")
	UAnimMontage* AttackMontageSword;

	UPROPERTY(EditDefaultsOnly, Category="Montages")
	UAnimMontage* AttackMontageAxes;

	UPROPERTY(EditDefaultsOnly, Category="Montages")
	UAnimMontage* AttackMontageFlyingSwords;

	UPROPERTY(EditDefaultsOnly, Category="Montages")
	UAnimMontage* AttackMontageHammer;

private:
	UPROPERTY(EditAnywhere, Category="Sounds")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category="Visual Effects")
	UParticleSystem* HitParticles;
};
