// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/// Enum type used to determine the state of vessels in terms of color.
UENUM(BlueprintType)
enum class EVesselState : uint8
{
	Unequipped  UMETA(DisplayName = "Unequipped"),
	Empty		UMETA(DisplayName = "Empty"),
	Cyan		UMETA(DisplayName = "Cyan"),
	Magenta		UMETA(DisplayName = "Magenta"),
	Yellow		UMETA(DisplayName = "Yellow"),
	Lime		UMETA(DisplayName = "Lime"),
	Orange		UMETA(DisplayName = "Orange"),
	Violet		UMETA(DisplayName = "Violet"),
	White		UMETA(DisplayName = "White")
};

/// Enum type used to determine the state of an activatable vessel ability.
UENUM(BlueprintType)
enum class EVesselAbilityState : uint8
{
	Idle		UMETA(DisplayName = "Idle"),
	Aiming		UMETA(DisplayName = "Aiming"),
	Charging	UMETA(DisplayName = "Charging"),
	Firing		UMETA(DisplayName = "Firing"),
};

UENUM(BlueprintType)
enum class EEffectShapeType : uint8
{
	Box			UMETA(DisplayName = "Box"),
	Capsule		UMETA(DisplayName = "Capsule"),
	Cone		UMETA(DisplayName = "Cone"),
	Sphere		UMETA(DisplayName = "Sphere"),
};
