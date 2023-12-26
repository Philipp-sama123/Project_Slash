// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Soul.h"
#include "Interfaces/PickupInterface.h"

#include "Kismet/KismetSystemLibrary.h"

void ASoul::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const double LocationZ = GetActorLocation().Z;
	if (LocationZ > DesiredZ)
	{
		const FVector DeltaLocation = FVector(0.f, 0.f, DriftRate * DeltaSeconds);
		AddActorWorldOffset(DeltaLocation);
	}
}

void ASoul::BeginPlay()
{
	Super::BeginPlay();

	FHitResult HitResult;

	const FVector Start = GetActorLocation();
	const FVector End = Start - FVector(0.f, 0.f, 150.f);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TArray<AActor*> ActorsToIgnore;

	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery1);
	ActorsToIgnore.Add(GetOwner());

	UKismetSystemLibrary::LineTraceSingleForObjects(
		this,
		Start,
		End,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		HitResult,
		true
	);
	DesiredZ = HitResult.ImpactPoint.Z + 25;
}

void ASoul::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                 int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor))
	{
		PickupInterface->AddSouls(this);

		SpawnPickupSystem();
		PlayPickupSound();
		Destroy();
	}
}
