// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Characters/CharacterTypes.h"

#include "Enemy.generated.h"

class ASoul;
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

	/** < AActor > */
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;
	virtual void Destroyed() override;
	/** </ AActor > */

	/** < IHitInterface > */
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	/** </ IHitInterface > */

protected:
	/** < AActor > */
	virtual void BeginPlay() override;
	/** </ AActor > */

	/** < BaseCharacter > */
	virtual bool CanAttack() override;
	virtual void AttackEnd() override;
	virtual void HandleDamage(float DamageAmount) override;
	virtual void Die_Implementation() override;
	/** </ BaseCharacter > */

	void SpawnSoul();

	UPROPERTY(VisibleAnywhere, Category="Combat")
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

private:
	/** AI Behavior */
	void InitializeEnemy();
	void CheckCombatTarget();
	void CheckPatrolTarget();
	void StartAttackTimer();
	void ClearAttackTimer();
	void ClearPatrolTimer();
	void PatrolTimerFinished();
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();

	void StartPatrolling();
	void ChaseTarget();
	void Attack();
	void LoseInterest();
	void SpawnDefaultWeapon();

	bool InTargetRange(AActor* Target, double Radius);
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsInsideAttackRadius();

	bool IsEngaged();
	bool IsAttacking();
	bool IsChasing();
	bool IsDead();

	void HideHealthBar();
	void ShowHealthBar();

	UAnimMontage* SelectCurrentAttackMontage();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn); // Callback for OnPawnSeen in UPawnSensingComponent

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

	UPROPERTY(EditAnywhere, Category="Combat")
	TSubclassOf<AWeapon> WeaponClass;

	UPROPERTY(EditInstanceOnly, Category="AI Navigation")
	AAIController* EnemyController;

	FTimerHandle PatrolTimer;

	FTimerHandle AttackTimer;

	UPROPERTY(EditInstanceOnly, Category="AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category="AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere, Category="AI Navigation")
	double PatrolRadius = 200.f;

	UPROPERTY(EditAnywhere, Category="AI Navigation")
	float PatrolWaitMin = 5.f;

	UPROPERTY(EditAnywhere, Category="AI Navigation")
	float PatrolWaitMax = 10.f;

	UPROPERTY(EditAnywhere, Category="Combat")
	float PatrollingSpeed = 100.f;

	UPROPERTY(EditAnywhere, Category="Combat")
	float AttackWaitMin = 0.5f;

	UPROPERTY(EditAnywhere, Category="Combat")
	float AttackWaitMax = 1.5f;

	UPROPERTY(EditAnywhere, Category="Combat")
	float ChasingSpeed = 250.f;

	UPROPERTY(EditAnywhere, Category="Combat")
	double CombatRadius = 1000.f;

	UPROPERTY(EditAnywhere, Category="Combat")
	double AttackRadius = 200.f;

	UPROPERTY(EditAnywhere, Category="Combat")
	double AcceptanceRadius = 60.f;;

	UPROPERTY(EditAnywhere, Category="Combat")
	float DeathLifeSpan = 10.f;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ASoul> SoulClass;
};
