// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Slash/DebugMakros.h"


AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemy::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void AEnemy::DirectionalHitReact(const FVector& ImpactPoint)
{
	const FVector ForwardVector = GetActorForwardVector();
	// Lower Impact Point to Enemy Location.Z
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();

	// Forward * ToHit = |Forward||ToHit| * cos(theta)
	// |Forward| = 1, |ToHit| = 1, soo ... Forward * ToHit = cos(theta);
	const double CosTheta = FVector::DotProduct(ForwardVector, ToHit);
	double Theta = FMath::Acos(CosTheta); // in radians (!)
	// convert to degrees
	Theta = FMath::RadiansToDegrees(Theta);

	//if Cross Product points down,Theta should be negative
	const FVector CrossProduct = FVector::CrossProduct(ForwardVector, ToHit);
	if (CrossProduct.Z < 0)
	{
		Theta *= -1;
	}

	FName Section("FromBack");
	if (Theta >= -45.F && Theta < 45.F)
	{
		Section = FName("FromFront");
	}
	else if (Theta >= -135.F && Theta < -45.F)
	{
		Section = FName("FromLeft");
	}
	else if (Theta >= 45.F && Theta < -135.F)
	{
		Section = FName("FromRight");
	}
	PlayHitReactMontage(Section);
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::GetHit(const FVector& ImpactPoint)
{
	// DRAW_SPHERE_COLOR(ImpactPoint, FColor::Orange);
	DirectionalHitReact(ImpactPoint);

	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			HitSound,
			ImpactPoint
		);
	}
	if (HitParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitParticles,
			ImpactPoint
		);
	}
}
