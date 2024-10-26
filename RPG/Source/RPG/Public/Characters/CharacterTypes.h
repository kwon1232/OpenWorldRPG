#pragma once

UENUM(BlueprintType)
enum class ECharacterEquipState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_EquippedOneHandeWeapon UMETA(DisplayName = "Equipped One-Handed Weapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two-Handed Weapon")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_HitReaction UMETA(DisplayName = "HitReaction"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_EquippingWeapon UMETA(DisplayName = "Equipping Weapon"),
	EAS_Dodge UMETA(DisplayName = "Dodge"),
	EAS_Riding UMETA(DisplayName = "Riding"),
	EAS_Dead UMETA(DisplayName = "Dead")
};

UENUM(BlueprintType)
enum class ECharacterRidingMosionState : uint8
{
	ERS_RidingIdle UMETA(DisplayName = "RidingIdle"),
	EMS_RidingWalk UMETA(DisplayName = "RidingWalk"),
	EMS_RidingTrot UMETA(DisplayName = "RidingTrot"),
	EMS_RidingGallop UMETA(DisplayName = "RidingGallop"),
	EMS_RidingBack UMETA(DisplayName = "RidingBack")
};


UENUM(BlueprintType)
enum class ECharacterMosionState : uint8
{
	EMS_StandingUp UMETA(DisplayName = "StandingUp"),
	EMS_Crouch UMETA(DisplayName = "Crouch"), // 쭈그리기
	EMS_Riding UMETA(DisplayName = "Riding") // 이동수단 이용
};

UENUM(BlueprintType)
enum EDeathPose
{
	EAS_Death1 UMETA(DisplayName = "Death1"),
	EAS_Death2 UMETA(DisplayName = "Death2"),
	EAS_Death3 UMETA(DisplayName = "Death3"),
	EAS_Death4 UMETA(DisplayName = "Death4"),
	EAS_Death5 UMETA(DisplayName = "Death5"),
	EAS_Death6 UMETA(DisplayName = "Death6"),

	EDP_MAX UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_NoState UMETA(DisplayName = "NoState"),

	EES_Dead UMETA(DisplayName = "Dead"),
	EES_Patrolling UMETA(DisplayName = "Patrolling"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_Attacking UMETA(DisplayName = "Attacking"),
	EES_Engaged UMETA(DisplayName = "Engaged")
};