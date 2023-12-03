// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Interfaces/HitInterface.h"
#include "NiagaraComponent.h"


AWeapon::AWeapon()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponBox->SetupAttachment(GetRootComponent());

	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());

	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);

	AttackMeshToSocket(InParent, InSocketName);
	ItemState = EItemState::EIS_Equipped;

	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			EquipSound,
			GetActorLocation()
		);
	}
	if (Sphere)
	{
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (EmbersEffect)
	{
		EmbersEffect->Deactivate();
	}
}

void AWeapon::AttackMeshToSocket(USceneComponent* InParent, FName InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}

void AWeapon::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                   int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("OnSphereBeginOverlap"));

	Super::OnSphereBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                 int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("OnSphereEndOverlap"));
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

// TODO THINK
void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
						   int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();

	FHitResult SphereHit;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}

	// ToDo: Replace with something more accurate (!)
	UKismetSystemLibrary::SphereTraceSingle(
		this,
		Start,
		End,
		15.f,  // Specify the radius of the sphere
		ETraceTypeQuery::TraceTypeQuery10,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::Persistent,
		SphereHit,
		true
	);

	if (SphereHit.GetActor())
	{
		UGameplayStatics::ApplyDamage(
			SphereHit.GetActor(),
			Damage,
			GetInstigator()->GetController(),
			this,
			UDamageType::StaticClass()
		);

		IHitInterface* HitInterface = Cast<IHitInterface>(SphereHit.GetActor());
		if (HitInterface)
		{
			HitInterface->Execute_GetHit(SphereHit.GetActor(), SphereHit.ImpactPoint);
		}

		IgnoreActors.AddUnique(SphereHit.GetActor());

		CreateFields(SphereHit.ImpactPoint);
	}
}

// TODO THINK
// void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
//                            int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
// {
// 	UE_LOG(LogTemp, Warning, TEXT("OnBoxOverlap"));
// 	const FVector Start = BoxTraceStart->GetComponentLocation();
// 	const FVector End = BoxTraceEnd->GetComponentLocation();
//
// 	FHitResult BoxHit;
// 	TArray<AActor*> ActorsToIgnore;
// 	ActorsToIgnore.Add(this);
//
// 	for (AActor* Actor : IgnoreActors)
// 	{
// 		ActorsToIgnore.AddUnique(Actor);
// 	}
//
// // ToDo: Replace with something more accurate (!)
// 	//
// 	// Replace it with a single check for the enemy and dont do additional stuff
//
// 	UKismetSystemLibrary::BoxTraceSingle(
// 		this,
// 		Start,
// 		End,
// 		FVector(5, 5, 5),
// 		BoxTraceStart->GetComponentRotation(),
// 		ETraceTypeQuery::TraceTypeQuery1,
// 		false,
// 		ActorsToIgnore,
// 		EDrawDebugTrace::ForDuration,
// 		BoxHit,
// 		true
// 	);
// 	if (BoxHit.GetActor())
// 	{
// 		
// 		UE_LOG(LogTemp, Warning, TEXT("OnBoxOverlap BoxHit.GetActor"));
// 		UGameplayStatics::ApplyDamage(
// 			BoxHit.GetActor(),
// 			Damage,
// 			GetInstigator()->GetController(),
// 			this,
// 			UDamageType::StaticClass()
// 		);
//
// 		IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
// 		if (HitInterface)
// 		{
// 			HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint);
// 		}
// 		IgnoreActors.AddUnique(BoxHit.GetActor());
//
// 		CreateFields(BoxHit.ImpactPoint);
// 	}
// }
// TODO THINK
// void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
//                            int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
// {
// 	const FVector Start = BoxTraceStart->GetComponentLocation();
// 	const FVector End = BoxTraceEnd->GetComponentLocation();
//
// 	TArray<FHitResult> HitResults;
// 	TArray<AActor*> ActorsToIgnore;
// 	ActorsToIgnore.Add(this);
//
// 	for (AActor* Actor : IgnoreActors)
// 	{
// 		ActorsToIgnore.AddUnique(Actor);
// 	}
//
// 	// ToDo: Replace with something m
// 	UKismetSystemLibrary::BoxTraceMulti(
// 		this,
// 		Start,
// 		End,
// 		FVector(10.f, 10.f, 10.f),
// 		BoxTraceStart->GetComponentRotation(),
// 		ETraceTypeQuery::TraceTypeQuery1,
// 		false,
// 		ActorsToIgnore,
// 		EDrawDebugTrace::ForDuration,
// 		HitResults,
// 		true
// 	);
//
// 	for (const FHitResult& HitResult : HitResults)
// 	{
// 		AActor* HitActor = HitResult.GetActor();
// 		if (HitActor)
// 		{
// 			UE_LOG(LogTemp, Warning, TEXT("OnBoxOverlap HitActor: %s"), *HitActor->GetName());
//
// 			UGameplayStatics::ApplyDamage(
// 				HitActor,
// 				Damage,
// 				GetInstigator()->GetController(),
// 				this,
// 				UDamageType::StaticClass()
// 			);
//
// 			IHitInterface* HitInterface = Cast<IHitInterface>(HitActor);
// 			if (HitInterface)
// 			{
// 				HitInterface->Execute_GetHit(HitActor, HitResult.ImpactPoint);
// 			}
//
// 			IgnoreActors.AddUnique(HitActor);
//
// 			CreateFields(HitResult.ImpactPoint);
// 		}
// 	}
// }
// TODO Think
// void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
//                            int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
// {
// 	UE_LOG(LogTemp, Warning, TEXT("OnBoxOverlap"));
// 	TArray<AActor*> ActorsToIgnore;
// 	ActorsToIgnore.Add(this);
//
// 	for (AActor* Actor : IgnoreActors)
// 	{
// 		ActorsToIgnore.AddUnique(Actor);
// 	}
//
//
// 	if (SweepResult.GetActor())
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("OnBoxOverlap BoxHit.GetActor"));
// 		UGameplayStatics::ApplyDamage(
// 			SweepResult.GetActor(),
// 			Damage,
// 			GetInstigator()->GetController(),
// 			this,
// 			UDamageType::StaticClass()
// 		);
//
// 		IHitInterface* HitInterface = Cast<IHitInterface>(SweepResult.GetActor());
// 		if (HitInterface)
// 		{
// 			HitInterface->Execute_GetHit(SweepResult.GetActor(), SweepResult.ImpactPoint);
// 		}
// 		IgnoreActors.AddUnique(SweepResult.GetActor());
//
// 		CreateFields(SweepResult.ImpactPoint);
// 	}
// }
