// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterTypes.h"
#include "GameFramework/Character.h"

#include "InputActionValue.h"
#include "SlashCharacter.generated.h"

class UInputMappingContext;
class UAnimMontage;
class UInputAction;

class USpringArmComponent;
class UCameraComponent;
class AItem;
class AWeapon;

UCLASS()
class SLASH_API ASlashCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASlashCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

protected:
	virtual void BeginPlay() override;

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

	/**
	 * Input Actions
	 */
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Equip(const FInputActionValue& Value);
	void Attack(const FInputActionValue& Value);

	/**
	 * Animation Montage Functions
	 */
	void PlayAttackMontage(UAnimMontage* CurrentAttackMontage);

	UFUNCTION(BlueprintCallable)
	void AttackEnd();
	void PlayEquipMontage(const FName& SectionName);

	bool CanAttack();
	bool CanDisarm();
	bool CanArm();

	UFUNCTION(BlueprintCallable)
	void Disarm();

	UFUNCTION(BlueprintCallable)
	void Arm();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

private:
	UAnimMontage* SelectCurrentAttackMontage() const;
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	UPROPERTY(VisibleAnywhere, Category=Weapon)
	AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, Category=Weapon)
	int32 CurrentAttackIndex = 0;

	/**
	 * Animation Montages
	 */
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
	UAnimMontage* EquipMontageSpear;

	UPROPERTY(EditDefaultsOnly, Category="Montages")
	UAnimMontage* EquipMontageSword;

	UPROPERTY(EditDefaultsOnly, Category="Montages")
	UAnimMontage* EquipMontageAxes;

	UPROPERTY(EditDefaultsOnly, Category="Montages")
	UAnimMontage* EquipMontageFlyingSwords;
	
	UPROPERTY(EditDefaultsOnly, Category="Montages")
	UAnimMontage* EquipMontageHammer;

public:
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE AWeapon* GetCurrentEquippedWeapon() const { return EquippedWeapon; }
};
