// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "BaseCharacter.generated.h"

enum EDeathPose : uint8;
class AWeapon;
class UAttributeComponent;
class UAnimMontage;


UCLASS()
class SLASH_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	/** < AActor > */
	virtual void Tick(float DeltaTime) override;
	/** </ AActor > */

	/** < IHitInterface > */
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	/** </ IHitInterface > */
protected:
	/** < AActor > */
	virtual void BeginPlay() override;
	/** </ AActor > */
	bool IsAlive();
	void DisableMeshCollision();
	void DisableCapsule();
	/** Combat */
	void DirectionalHitReact(const FVector& ImpactPoint);
	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitParticles(const FVector& ImpactPoint);
	virtual void Die();
	virtual bool CanAttack();
	virtual void HandleDamage(float DamageAmount);
	/** Montages */
	void PlayHitReactMontage(const FName& SectionName);
	void StopAttackMontage();
	virtual int32 PlayAttackMontage(UAnimMontage* CurrentAttackMontage);
	virtual int32 PlayDeathMontage();

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();

	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	UFUNCTION(BlueprintCallable)
	virtual void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

	UPROPERTY(VisibleAnywhere, Category=Weapon)
	AWeapon* EquippedWeapon;

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

	UPROPERTY(EditDefaultsOnly, Category="Montages")
	UAnimMontage* AttackMontageFists;

	UPROPERTY(BlueprintReadOnly, Category="Combat")
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere, Category="Combat")
	double WarpTargetDistance = 75.f;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;

private:
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);
	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);

	UPROPERTY(EditDefaultsOnly, Category="Montages")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category="Montages")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category="Combat")
	TArray<FName> AttackMontageSections;

	UPROPERTY(EditAnywhere, Category="Combat")
	TArray<FName> DeathMontageSections;

	UPROPERTY(EditAnywhere, Category="Sounds")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category="Visual Effects")
	UParticleSystem* HitParticles;

public:
	FORCEINLINE TEnumAsByte<EDeathPose> GetDeathPose() const { return DeathPose; }
};
