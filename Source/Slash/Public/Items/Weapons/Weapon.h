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
	EWT_Fists,
	EWT_Sword,
	EWT_Spear,
	EWT_Axes,
	EWT_FlyingSwords,
	EWT_Hammer,
};

UCLASS()
class SLASH_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();
	void AttackMeshToSocket(USceneComponent* InParent, FName InSocketName);
	void ExecuteGetHit(FHitResult SphereHit);
	bool IsActorSameType(AActor* OtherActor);
	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);
	TArray<AActor*> IgnoreActors;

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);

protected:
	virtual void BeginPlay() override;
	void PlayEquipSound();
	void DisableSphereCollision();
	void DisableEmbersEffect();


	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                  int32 OtherBodyIndex,
	                  bool bFromSweep, const FHitResult& SweepResult);

private:
	void PerformSphereTrace(FHitResult& SphereHit);
	void PerformBoxTrace(FHitResult& BoxHit);

	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	float SphereTraceRadius = 15.f;

	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	FVector BoxTraceExtent = FVector(5.f);

	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	bool bShowBoxDebug = false;

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

	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	float Damage = 20.F;

public:
	FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox; }
	FORCEINLINE void SetWeaponType(EWeaponType Type) { WeaponType = Type; }
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE USceneComponent* GetBoxTraceStart() const { return BoxTraceStart; }
	FORCEINLINE USceneComponent* GetBoxTraceEnd() const { return BoxTraceEnd; }
	FORCEINLINE float GetDamage() const { return Damage; }
};
