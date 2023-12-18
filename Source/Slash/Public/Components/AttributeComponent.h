// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SLASH_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void RegenerateStamina(float DeltaTime);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category="Actor Attributes")
	float Health;

	UPROPERTY(EditAnywhere, Category="Actor Attributes")
	float MaxHealth;

	UPROPERTY(EditAnywhere, Category="Actor Attributes")
	float Stamina;

	UPROPERTY(EditAnywhere, Category="Actor Attributes")
	float MaxStamina;

	UPROPERTY(EditAnywhere, Category="Actor Attributes")
	float StaminaRegenerationRate = 8.f;

	UPROPERTY(EditAnywhere, Category="Actor Attributes")
	float DodgeCost = 10.f;

	UPROPERTY(EditAnywhere, Category="Actor Attributes")
	int32 Gold;

	UPROPERTY(EditAnywhere, Category="Actor Attributes")
	int32 Souls;

public:
	void ReceiveDamage(float Damage);
	void UseStamina(float StaminaCost);
	float GetHealthPercent();
	float GetStaminaPercent();
	bool IsAlive();

	void AddSouls(int32 SoulsAmount);
	void AddGold(int32 GoldAmount);

	FORCEINLINE int32 GetGold() const { return Gold; }
	FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE float GetDodgeCost() const { return DodgeCost; }
	FORCEINLINE float GetStamina() const { return Stamina; }
};
