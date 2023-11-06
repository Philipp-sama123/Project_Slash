#pragma once

UENUM(BlueprintType)
enum class ECharacterState: uint8
{
	ECS_Unequipped UMETA(DisplayName="Unequipped"),
	ECS_EquippedSpear UMETA(DisplayName="EquippedSpear"),
	ECS_EquippedAxes UMETA(DisplayName="EquippedAxes"),
	ECS_EquippedSword UMETA(DisplayName="EquippedSword"),
	ECS_EquippedFlyingSwords UMETA(DisplayName="EquippedFlyingSwords")
};

UENUM(BlueprintType)
enum class EActionState: uint8
{
	EAS_Unoccupied UMETA(DisplayName="Unoccupied"),
	EAS_Attacking UMETA(DisplayName="Attacking"),
	EAS_EquippingWeapon UMETA(DisplayName="Equipping Weapon")
};
