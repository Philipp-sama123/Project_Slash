// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"
#include "Components/InputComponent.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Components/BoxComponent.h"

ASlashCharacter::ASlashCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 200.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Move);
		EnhancedInputComponent->BindAction(LookingAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &ASlashCharacter::Equip);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ASlashCharacter::Attack);
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

	Tags.Add(FName("SlashCharacter"));
}

void ASlashCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;

	EquippedWeapon->IgnoreActors.Empty();
}

bool ASlashCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

void ASlashCharacter::Move(const FInputActionValue& Value)
{
	// ToDo: think of running and equip
	// ToDo: think of running attacks 
	if (ActionState != EActionState::EAS_Unoccupied) return;

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

void ASlashCharacter::EquipWeapon(AWeapon* Weapon)
{
	switch (Weapon->GetWeaponType())
	{
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
	default:
		return nullptr;
	}
}
