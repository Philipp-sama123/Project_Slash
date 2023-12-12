// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimationNotify/AnimNS_AttackTrigger.h"

#include "Characters/SlashCharacter.h"
#include "Interfaces/HitInterface.h"
#include "Items/Weapons/Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"


void UAnimNS_AttackTrigger::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
                                       const FAnimNotifyEventReference& EventReference)
{
	if (const ASlashCharacter* Character = Cast<ASlashCharacter>(MeshComp->GetOwner()))
	{
		if (AWeapon* Weapon = Character->GetCurrentEquippedWeapon())
		{
			const FVector Start = Weapon->GetBoxTraceStart()->GetComponentLocation();
			const FVector End = Weapon->GetBoxTraceEnd()->GetComponentLocation();

			FHitResult SphereHit;
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(Weapon->GetOwner());

			for (AActor* Actor : Weapon->IgnoreActors)
			{
				ActorsToIgnore.AddUnique(Actor);
			}

			UKismetSystemLibrary::SphereTraceSingle(
				MeshComp->GetWorld(),
				Start,
				End,
				15.f, // ToDo: Specify the radius of the sphere maybe depending on the Weapon
				ETraceTypeQuery::TraceTypeQuery1,
				false,
				ActorsToIgnore,
				EDrawDebugTrace::None,
				SphereHit,
				true
			);

			if (SphereHit.GetActor())
			{
				UGameplayStatics::ApplyDamage(
					SphereHit.GetActor(),
					Weapon->GetDamage(),
					MeshComp->GetOwner()->GetInstigator()->GetController(),
					MeshComp->GetOwner()->GetOwner(),
					UDamageType::StaticClass()
				);

				IHitInterface* HitInterface = Cast<IHitInterface>(SphereHit.GetActor());
				if (HitInterface)
				{
					HitInterface->Execute_GetHit(SphereHit.GetActor(), SphereHit.ImpactPoint, MeshComp->GetOwner());
				}

				Weapon->IgnoreActors.AddUnique(SphereHit.GetActor());
				Weapon->CreateFields(SphereHit.ImpactPoint);
			}
		}
	}
}
