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
class UInputComponent;
class AWeapon;

UCLASS()
class SLASH_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;
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

	FTimerHandle PatrolTimer;

	void PatrolTimerFinished();

	UPROPERTY(EditInstanceOnly, Category="AI Navigation")
	AAIController* EnemyController;

	UPROPERTY(EditInstanceOnly, Category="AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category="AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere, Category="AI Navigation")
	double PatrolRadius = 200.f;

	UPROPERTY(EditAnywhere, Category="AI Navigation")
	float WaitMin = 5.f;

	UPROPERTY(EditAnywhere, Category="AI Navigation")
	float WaitMax = 10.f;
	/**
	 * AI Behavior
	 */
	void HideHealthBar();
	void ShowHealthBar();
	void LoseInterest();
	void StartPatrolling();
	void ChaseTarget();
	void ClearPatrolTimer();

	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsInsideAttackRadius();
	bool IsChasing();
	bool IsAttacking();
	bool IsEngaged();
	bool IsDead();
	/**
	 * Combat
	 */
	void StartAttackTimer();
	void ClearAttackTimer();

	FTimerHandle AttackTimer;
	UPROPERTY(EditAnywhere, Category="Combat")
	float AttackMin = 0.5f;

	UPROPERTY(EditAnywhere, Category="Combat")
	float AttackMax = 2.5f;

	UPROPERTY(EditAnywhere, Category="Combat")
	float PatrollingSpeed = 100.f;

	UPROPERTY(EditAnywhere, Category="Combat")
	float ChasingSpeed = 250.f;

protected:
	virtual void BeginPlay() override;
	virtual void Die() override;
	virtual bool CanAttack() override;
	virtual void HandleDamage(float DamageAmount) override;
	virtual int32 PlayDeathMontage() override;

	UPROPERTY(EditAnywhere, Category="Combat")
	float DeathLifeSpan = 5.f;

	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);

	void CheckCombatTarget();
	void CheckPatrolTarget();
	void Attack();

	UAnimMontage* SelectCurrentAttackMontage();
	AActor* ChoosePatrolTarget();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;

	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;
};
