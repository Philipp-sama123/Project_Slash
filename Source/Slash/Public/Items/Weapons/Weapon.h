// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"
class USoundBase;
class UBoxComponent;
class USceneComponent;

UENUM(BlueprintType)
enum class EWeaponType: uint8
{
	EWT_Sword,
	EWT_Spear,
	EWT_Axes,
	EWT_FlyingSwords,
};

UCLASS()
class SLASH_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();
	void AttackMeshToSocket(USceneComponent* InParent, FName InSocketName);
	void Equip(USceneComponent* InParent, FName InSocketName);
	TArray<AActor*> IgnoreActors;

protected:
	virtual void BeginPlay() override;

	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                                  int32 OtherBodyIndex,
	                                  bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                                int32 OtherBodyIndex) override;

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                  int32 OtherBodyIndex,
	                  bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	USoundBase* EquipSound;

	UPROPERTY(VisibleAnywhere, Category="Weapon Properties")
	UBoxComponent* WeaponBox;

	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	EWeaponType WeaponType;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceStart;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceEnd;

public:
	FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox; }
	FORCEINLINE void SetWeaponType(EWeaponType Type) { WeaponType = Type; }
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
};
