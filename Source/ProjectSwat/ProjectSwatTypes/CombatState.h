#pragma once

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),
	ECS_TossGrenade UMETA(DisplayName = "Toss Grenade"),

	ECS_MAX UMETA(DisplayName = "DefaultMAX")
};