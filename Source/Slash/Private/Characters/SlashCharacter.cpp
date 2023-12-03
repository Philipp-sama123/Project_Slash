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
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);


	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
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

void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
		switch (OverlappingWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_Axes:
			OverlappingWeapon->Equip(GetMesh(), FName("WEAPON_R"), this, this);
			CharacterState = ECharacterState::ECS_EquippedAxes;
			break;

		case EWeaponType::EWT_Spear:
			OverlappingWeapon->Equip(GetMesh(), FName("WEAPON_R"), this, this);
			CharacterState = ECharacterState::ECS_EquippedSpear;
			break;

		case EWeaponType::EWT_Sword:
			OverlappingWeapon->Equip(GetMesh(), FName("WEAPON_R"), this, this);
			CharacterState = ECharacterState::ECS_EquippedSword;
			break;

		case EWeaponType::EWT_FlyingSwords:
			OverlappingWeapon->Equip(GetMesh(), FName("WEAPON_R"), this, this);
			CharacterState = ECharacterState::ECS_EquippedFlyingSwords;
			break;
		case EWeaponType::EWT_Hammer:
			OverlappingWeapon->Equip(GetMesh(), FName("WEAPON_R"), this, this);
			CharacterState = ECharacterState::ECS_EquippedHammer;
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("No WeaponType matching!"));
			break;
		}

		EquippedWeapon = OverlappingWeapon;
		OverlappingItem = nullptr;
	}
	else
	{
		if (CanDisarm())
		{
			UE_LOG(LogTemp, Warning, TEXT("WE HAVE NOOOOOOOOOOOOOO OVERLAPPING ITEM :O!"));
			PlayEquipMontage(FName("Unequip"));
			CharacterState = ECharacterState::ECS_Unequipped;
			ActionState = EActionState::EAS_EquippingWeapon;
		}
		else if (CanArm())
		{
			PlayEquipMontage(FName("Equip"));
			CharacterState = ECharacterState::ECS_EquippedSpear;
			ActionState = EActionState::EAS_EquippingWeapon;
		}
	}
}

void ASlashCharacter::PlayAttackMontage(UAnimMontage* CurrentAttackMontage)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CurrentAttackMontage)
	{
		AnimInstance->Montage_Play(CurrentAttackMontage);
		//ToDo: make all attacks work -> currently just 2-4 --  int32 Selection = FMath::RandRange(0, 7);
		// int32 Selection = FMath::RandRange(0, 3);

		FName SectionName = FName();
		switch (CurrentAttackIndex)
		{
		case 0:
			SectionName = FName("Attack1");
			break;
		case 1:
			SectionName = FName("Attack2");
			break;
		case 2:
			SectionName = FName("Attack3");
			break;
		case 3:
			SectionName = FName("Attack4");
			CurrentAttackIndex = -1;
			break;
		default:
			CurrentAttackIndex = -1;
			break;
		}
		CurrentAttackIndex = CurrentAttackIndex += 1;
		AnimInstance->Montage_JumpToSection(SectionName, CurrentAttackMontage);
	}
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


void ASlashCharacter::Disarm()
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

void ASlashCharacter::Arm()
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

void ASlashCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::Attack(const FInputActionValue& Value)
{
	if (CanAttack())
	{
		PlayAttackMontage(SelectCurrentAttackMontage());
		ActionState = EActionState::EAS_Attacking;
	}
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

void ASlashCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
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
	default:
		return nullptr;
	}
}

bool ASlashCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
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
