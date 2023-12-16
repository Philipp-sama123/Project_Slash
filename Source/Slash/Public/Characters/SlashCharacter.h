// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "CharacterTypes.h"
#include "GameFramework/Character.h"

#include "InputActionValue.h"
#include "SlashCharacter.generated.h"

class USlashOverlay;
class USphereComponent;
class UInputMappingContext;
class UInputComponent;
class UInputAction;

class UAnimMontage;
class USpringArmComponent;
class UCameraComponent;
class AItem;
class AWeapon;

UCLASS()
class SLASH_API ASlashCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	ASlashCharacter();
	/** < AActor >*/
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	/** < ACharacter >*/
	virtual void Jump() override;
	/** </ ACharacter >*/
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;
	/** </ AActor >*/

	/** < ABaseCharacter >*/
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	/** </ ABaseCharacter >*/
protected:
	/** < AActor >*/
	virtual void BeginPlay() override;
	/** </ AActor >*/

	/** < ABaseCharacter >*/
	virtual void AttackEnd() override;
	virtual bool CanAttack() override;
	virtual void Die() override;
	/** </ ABaseCharacter >*/

	/** Input Actions */
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Equip(const FInputActionValue& Value);
	void Attack(const FInputActionValue& Value);

	void EquipWeapon(AWeapon* Weapon);
	void PlayEquipMontage(const FName& SectionName);
	bool CanDisarm();
	bool CanArm();
	void Arm();
	void Disarm();
	void SelectClosestCombatTarget();

	UFUNCTION()
	void OnCombatTargetSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                                      int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnCombatTargetSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                                    int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void AttachWeaponOnBack();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponOnHand();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

	UPROPERTY(VisibleAnywhere, Category="Combat")
	TArray<AActor*> CombatTargetsInRange;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputMappingContext* SlashContext;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MovementAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookingAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* EquipAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* AttackAction;

	UPROPERTY()
	USlashOverlay* SlashOverlay;

private:
	void SetHUDHealth();
	void InitializeSlashOverlay();
	UAnimMontage* SelectCurrentAttackMontage() const;
	bool IsUnoccupied();

	/** Character Components */
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* CombatTargetSphereComponent;

	UPROPERTY(EditDefaultsOnly, Category="Montages")
	UAnimMontage* EquipMontageSpear;

	UPROPERTY(EditDefaultsOnly, Category="Montages")
	UAnimMontage* EquipMontageSword;

	UPROPERTY(EditDefaultsOnly, Category="Montages")
	UAnimMontage* EquipMontageAxes;

	UPROPERTY(EditDefaultsOnly, Category="Montages")
	UAnimMontage* EquipMontageFlyingSwords;

	UPROPERTY(EditDefaultsOnly, Category="Montages")
	UAnimMontage* EquipMontageHammer;

	UPROPERTY(EditDefaultsOnly, Category="Montages")
	UAnimMontage* EquipMontageFists;

	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;
	EActionState ActionState = EActionState::EAS_Unoccupied;

public:
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
	FORCEINLINE AWeapon* GetCurrentEquippedWeapon() const { return EquippedWeapon; }
};
