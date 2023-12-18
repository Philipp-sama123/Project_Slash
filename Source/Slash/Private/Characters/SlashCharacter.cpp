// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"
#include "Components/InputComponent.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/AttributeComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Components/SphereComponent.h"
#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"
#include "Items/Soul.h"
#include "Items/Treasure.h"

ASlashCharacter::ASlashCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 200.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetMesh()->SetCollisionObjectType(ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	CombatTargetSphereComponent = CreateDefaultSubobject<USphereComponent>("CombatTargetSphereComponent");
	CombatTargetSphereComponent->SetupAttachment(GetRootComponent());
	CombatTargetSphereComponent->SetSphereRadius(125.f);
}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Move);
		EnhancedInputComponent->BindAction(LookingAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ASlashCharacter::Jump);
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &ASlashCharacter::Equip);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ASlashCharacter::Attack);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &ASlashCharacter::Dodge);
	}
}

void ASlashCharacter::Jump()
{
	if (IsUnoccupied())
	{
		Super::Jump();
	}
}

float ASlashCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	SetHUDHealth();
	return DamageAmount;
}

void ASlashCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	if (Attributes && Attributes->GetHealthPercent() > 0.f)
	{
		ActionState = EActionState::EAS_HitReaction;
	}
}

void ASlashCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void ASlashCharacter::AddSouls(ASoul* Soul)
{
	if (Attributes && SlashOverlay)
	{
		Attributes->AddSouls(Soul->GetSouls());
		SlashOverlay->SetSoulsCount(Attributes->GetSouls());
	}
}

void ASlashCharacter::AddGold(ATreasure* Treasure)
{
	if (Attributes && SlashOverlay)
	{
		Attributes->AddGold(Treasure->GetGold());
		SlashOverlay->SetGoldCount(Attributes->GetGold());
	}
}

void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (const APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
			PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(SlashContext, 0);
		}
	}

	CombatTargetSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ASlashCharacter::OnCombatTargetSphereBeginOverlap);
	CombatTargetSphereComponent->OnComponentEndOverlap.AddDynamic(this, &ASlashCharacter::OnCombatTargetSphereEndOverlap);

	Tags.Add(FName("EngageableTarget"));

	InitializeSlashOverlay();
}

void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Attributes && SlashOverlay)
	{
		Attributes->RegenerateStamina(DeltaTime);
		SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void ASlashCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;

	if (EquippedWeapon)
	{
		EquippedWeapon->IgnoreActors.Empty();
	}
}

void ASlashCharacter::DodgeEnd()
{
	Super::DodgeEnd();
	ActionState = EActionState::EAS_Unoccupied;
}

bool ASlashCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

void ASlashCharacter::Die()
{
	Super::Die();

	DisableMeshCollision();
	ActionState = EActionState::EAS_Dead;
}

void ASlashCharacter::Move(const FInputActionValue& Value)
{
	// ToDo: think of running and equip
	// ToDo: think of running attacks 
	if (IsOccupied()) return;

	const FVector2d MovementVector = Value.Get<FVector2d>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MovementVector.Y);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void ASlashCharacter::Look(const FInputActionValue& Value)
{
	const FVector2d LookAxisValue = Value.Get<FVector2d>();

	AddControllerPitchInput(LookAxisValue.Y);
	AddControllerYawInput(LookAxisValue.X);
}

void ASlashCharacter::Equip(const FInputActionValue& Value)
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);

	if (OverlappingWeapon)
	{
		EquipWeapon(OverlappingWeapon);
	}
	else
	{
		if (CanDisarm())
		{
			Disarm();
		}
		else if (CanArm())
		{
			Arm();
		}
	}
}

void ASlashCharacter::Attack(const FInputActionValue& Value)
{
	if (CanAttack())
	{
		PlayAttackMontage(SelectCurrentAttackMontage());
		ActionState = EActionState::EAS_Attacking;
	}
}

bool ASlashCharacter::IsOccupied()
{
	return ActionState != EActionState::EAS_Unoccupied;
}

bool ASlashCharacter::HasEnoughStaminaToDodge()
{
	return Attributes && Attributes->GetStamina() > Attributes->GetDodgeCost();
}

void ASlashCharacter::Dodge(const FInputActionValue& Value)
{
	if (IsOccupied() || !HasEnoughStaminaToDodge()) return;

	PlayDodgeMontage();

	if (Attributes && SlashOverlay)
	{
		Attributes->UseStamina(Attributes->GetDodgeCost());
		SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}

	ActionState = EActionState::EAS_Dodge;
}

void ASlashCharacter::EquipWeapon(AWeapon* Weapon)
{
	switch (Weapon->GetWeaponType())
	{
	case EWeaponType::EWT_Fists:
		Weapon->Equip(GetMesh(), FName("WEAPON_R"), this, this); // and Left?
		CharacterState = ECharacterState::ECS_EquippedFists;
		break;

	case EWeaponType::EWT_Axes:
		Weapon->Equip(GetMesh(), FName("WEAPON_R"), this, this);
		CharacterState = ECharacterState::ECS_EquippedAxes;
		break;

	case EWeaponType::EWT_Spear:
		Weapon->Equip(GetMesh(), FName("WEAPON_R"), this, this);
		CharacterState = ECharacterState::ECS_EquippedSpear;
		break;

	case EWeaponType::EWT_Sword:
		Weapon->Equip(GetMesh(), FName("WEAPON_R"), this, this);
		CharacterState = ECharacterState::ECS_EquippedSword;
		break;

	case EWeaponType::EWT_FlyingSwords:
		Weapon->Equip(GetMesh(), FName("WEAPON_R"), this, this);
		CharacterState = ECharacterState::ECS_EquippedFlyingSwords;
		break;
	case EWeaponType::EWT_Hammer:
		Weapon->Equip(GetMesh(), FName("WEAPON_R"), this, this);
		CharacterState = ECharacterState::ECS_EquippedHammer;
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("No WeaponType matching!"));
		break;
	}

	OverlappingItem = nullptr;
	EquippedWeapon = Weapon;
}

void ASlashCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquippedWeapon)
	{
		switch (EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_Axes:
			if (EquipMontageAxes)
			{
				AnimInstance->Montage_Play(EquipMontageAxes);
				AnimInstance->Montage_JumpToSection(SectionName, EquipMontageAxes);
			}
			break;
		case EWeaponType::EWT_Spear:
			if (EquipMontageSpear)
			{
				AnimInstance->Montage_Play(EquipMontageSpear);
				AnimInstance->Montage_JumpToSection(SectionName, EquipMontageSpear);
			}
			break;
		case EWeaponType::EWT_Sword:
			if (EquipMontageSword)
			{
				AnimInstance->Montage_Play(EquipMontageSword);
				AnimInstance->Montage_JumpToSection(SectionName, EquipMontageSword);
			}
			break;
		case EWeaponType::EWT_FlyingSwords:
			if (EquipMontageFlyingSwords)
			{
				AnimInstance->Montage_Play(EquipMontageFlyingSwords);
				AnimInstance->Montage_JumpToSection(SectionName, EquipMontageFlyingSwords);
			}
			break;
		case EWeaponType::EWT_Hammer:
			if (EquipMontageHammer)
			{
				AnimInstance->Montage_Play(EquipMontageHammer);
				AnimInstance->Montage_JumpToSection(SectionName, EquipMontageHammer);
			}
			break;
		case EWeaponType::EWT_Fists:
			if (EquipMontageFists)
			{
				AnimInstance->Montage_Play(EquipMontageFists);
				AnimInstance->Montage_JumpToSection(SectionName, EquipMontageFists);
			}
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("No CharacterState matching!"));
			break;
		}
	}
}

bool ASlashCharacter::CanDisarm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

bool ASlashCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_Unequipped &&
		EquippedWeapon;
}

void ASlashCharacter::Arm()
{
	PlayEquipMontage(FName("Equip"));
	ActionState = EActionState::EAS_EquippingWeapon;
}

void ASlashCharacter::Disarm()
{
	PlayEquipMontage(FName("Unequip"));
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void ASlashCharacter::SelectClosestCombatTarget()
{
	if (CombatTargetsInRange.Num() > 0)
	{
		const FVector MyLocation = GetActorLocation();
		FVector ClosestEnemyDistance(10000.f);
		AActor* ClosestEnemy{};
		for (const auto CombatTargetInRange : CombatTargetsInRange)
		{
			FVector DistanceToEnemy = MyLocation - CombatTargetInRange->GetActorLocation();
			if (DistanceToEnemy.Length() < ClosestEnemyDistance.Length())
			{
				ClosestEnemy = CombatTargetInRange;
				ClosestEnemyDistance = DistanceToEnemy;
			}
		}
		CombatTarget = ClosestEnemy;
	}
}

void ASlashCharacter::OnCombatTargetSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                       const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag(FName("Enemy")))
	{
		CombatTargetsInRange.AddUnique(OtherActor);
		SelectClosestCombatTarget();
	}
}

void ASlashCharacter::OnCombatTargetSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (CombatTarget == OtherActor)
	{
		CombatTarget = nullptr;
	}
	if (CombatTargetsInRange.Contains(OtherActor))
	{
		CombatTargetsInRange.Remove(OtherActor);
	}
}

void ASlashCharacter::AttachWeaponOnBack()
{
	if (EquippedWeapon)
	{
		switch (EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_Axes:
			EquippedWeapon->AttackMeshToSocket(GetMesh(), FName("SPEAR_SOCKET_BACK"));
			break;
		case EWeaponType::EWT_Spear:
			EquippedWeapon->AttackMeshToSocket(GetMesh(), FName("SPEAR_SOCKET_BACK"));
			break;
		case EWeaponType::EWT_Sword:
			EquippedWeapon->AttackMeshToSocket(GetMesh(), FName("SPEAR_SOCKET_BACK"));
			break;
		case EWeaponType::EWT_FlyingSwords:
			EquippedWeapon->AttackMeshToSocket(GetMesh(), FName("SPEAR_SOCKET_BACK"));
			break;
		case EWeaponType::EWT_Hammer:
			EquippedWeapon->AttackMeshToSocket(GetMesh(), FName("SPEAR_SOCKET_BACK"));
			break;
		case EWeaponType::EWT_Fists:
			EquippedWeapon->AttackMeshToSocket(GetMesh(), FName("SPEAR_SOCKET_BACK"));
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("Disarm() CALLED NO STATE matching!"));
			break;
		}
	}
}

void ASlashCharacter::AttachWeaponOnHand()
{
	if (EquippedWeapon)
	{
		switch (EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_Fists:
			EquippedWeapon->AttackMeshToSocket(GetMesh(), FName("WEAPON_R"));
			CharacterState = ECharacterState::ECS_EquippedFists;
			break;
		case EWeaponType::EWT_Axes:
			EquippedWeapon->AttackMeshToSocket(GetMesh(), FName("WEAPON_R"));
			CharacterState = ECharacterState::ECS_EquippedAxes;
			break;
		case EWeaponType::EWT_Spear:
			EquippedWeapon->AttackMeshToSocket(GetMesh(), FName("WEAPON_R"));
			CharacterState = ECharacterState::ECS_EquippedSpear;
			break;
		case EWeaponType::EWT_Sword:
			EquippedWeapon->AttackMeshToSocket(GetMesh(), FName("WEAPON_R"));
			CharacterState = ECharacterState::ECS_EquippedSword;
			break;
		case EWeaponType::EWT_FlyingSwords:
			EquippedWeapon->AttackMeshToSocket(GetMesh(), FName("WEAPON_R"));
			CharacterState = ECharacterState::ECS_EquippedFlyingSwords;
			break;
		case EWeaponType::EWT_Hammer:
			EquippedWeapon->AttackMeshToSocket(GetMesh(), FName("WEAPON_R"));
			CharacterState = ECharacterState::ECS_EquippedHammer;
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("Arm() CALLED NO STATE matching!"));
			break;
		}
	}
}

void ASlashCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::SetHUDHealth()
{
	if (SlashOverlay && Attributes)
	{
		SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

void ASlashCharacter::InitializeSlashOverlay()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		ASlashHUD* SlashHUD = Cast<ASlashHUD>(PlayerController->GetHUD());
		if (SlashHUD)
		{
			SlashOverlay = SlashHUD->GetSlashOverlay();
			if (SlashOverlay && Attributes)
			{
				SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
				SlashOverlay->SetStaminaBarPercent(1);
				SlashOverlay->SetGoldCount(1);
				SlashOverlay->SetSoulsCount(1);
			}
		}
	}
}

UAnimMontage* ASlashCharacter::SelectCurrentAttackMontage() const
{
	switch (CharacterState)
	{
	case ECharacterState::ECS_EquippedAxes:
		return AttackMontageAxes;
	case ECharacterState::ECS_EquippedSpear:
		return AttackMontageSpear;
	case ECharacterState::ECS_EquippedSword:
		return AttackMontageSword;
	case ECharacterState::ECS_EquippedFlyingSwords:
		return AttackMontageFlyingSwords;
	case ECharacterState::ECS_EquippedHammer:
		return AttackMontageHammer;
	case ECharacterState::ECS_EquippedFists:
		return AttackMontageFists;
	default:
		return nullptr;
	}
}

bool ASlashCharacter::IsUnoccupied()
{
	return ActionState == EActionState::EAS_Unoccupied;
}
