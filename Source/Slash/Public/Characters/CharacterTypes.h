#pragma once

UENUM(BlueprintType)
enum class ECharacterState: uint8
{
	ECS_Unequipped UMETA(DisplayName="Unequipped"),
	ECS_EquippedFists UMETA(DisplayName="ECS_EquippedFists"),
	ECS_EquippedSpear UMETA(DisplayName="EquippedSpear"),
	ECS_EquippedAxes UMETA(DisplayName="EquippedAxes"),
	ECS_EquippedSword UMETA(DisplayName="EquippedSword"),
	ECS_EquippedFlyingSwords UMETA(DisplayName="EquippedFlyingSwords"),
	ECS_EquippedHammer UMETA(DisplayName="EquippedHammer")
};

UENUM(BlueprintType)
enum class EActionState: uint8
{
	EAS_Unoccupied UMETA(DisplayName="Unoccupied"),
	EAS_HitReaction UMETA(DisplayName="HitReaction"),
	EAS_Attacking UMETA(DisplayName="Attacking"),
	EAS_EquippingWeapon UMETA(DisplayName="Equipping Weapon")
};

UENUM(BlueprintType)
enum EDeathPose: uint8
{
	EDP_Death1 UMETA(DisplayName="Dead 01"),
	EDP_Death2 UMETA(DisplayName="Dead 02"),
	EDP_Death3 UMETA(DisplayName="Dead 03"),
	EDP_Death4 UMETA(DisplayName="Dead 04"),
	EDP_Death5 UMETA(DisplayName="Dead 05"),
	EDP_Death6 UMETA(DisplayName="Dead 06"),
	EDP_Death7 UMETA(DisplayName="Dead 07"),
	EDP_Death8 UMETA(DisplayName="Dead 08"),

	EDP_MAX UMETA(DisplayName="Default Max")
};

UENUM(BlueprintType)
enum class EEnemyState: uint8
{
	EES_Dead UMETA(DisplayName="Dead"),
	EES_Patrolling UMETA(DisplayName="Patrolling"),
	EES_Chasing UMETA(DisplayName="Chasing"),
	EES_Attacking UMETA(DisplayName="Attacking"),
	EES_Engaged UMETA(DisplayName="Engaged"),

	EES_NoState UMETA(DisplayName="No State")
};
