// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Characters/CharacterTypes.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"

#include "Enemy.generated.h"

class UPawnSensingComponent;
class UHealthBarComponent;
class UAttributeComponent;
class AAIController;
class AWeapon;

UCLASS()
class SLASH_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;
	virtual void Destroyed() override;

private:
	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> WeaponClass;

	UPROPERTY(VisibleAnywhere, Category="Combat")
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere, Category="Combat")
	double CombatRadius = 500.f;

	UPROPERTY(EditAnywhere, Category="Combat")
	double AttackRadius = 150.f;

	/**
	 * Navigation
	 */
	UPROPERTY(EditInstanceOnly, Category="AI Navigation")
	AAIController* EnemyController;

	UPROPERTY(EditInstanceOnly, Category="AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category="AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere, Category="AI Navigation")
	double PatrolRadius = 200.f;

	FTimerHandle PatrolTimer;
	void PatrolTimerFinished();

	UPROPERTY(EditAnywhere, Category="AI Navigation")
	float WaitMin = 5.f;
	UPROPERTY(EditAnywhere, Category="AI Navigation")
	float WaitMax = 10.f;

	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

protected:
	virtual void BeginPlay() override;
	virtual void Die() override;
	virtual void PlayAttackMontage(UAnimMontage* CurrentAttackMontage) override;
	void Attack();
	UAnimMontage* SelectCurrentAttackMontage();
	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();
	void CheckCombatTarget();
	void CheckPatrolTarget();


	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);

	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose = EDeathPose::EDP_Alive;
};
