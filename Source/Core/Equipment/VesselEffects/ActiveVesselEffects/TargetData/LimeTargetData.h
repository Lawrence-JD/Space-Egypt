// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveTargetData.h"
#include "LimeTargetData.generated.h"

UENUM(BlueprintType)
enum class EChargingState : uint8
{
	Idle		UMETA(DisplayName = "Idle"),
	Charging	UMETA(DisplayName = "Charging"),
	Depleteing	UMETA(DisplayName = "Depleteing"),
};

UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent), NonTransient)
class SPEEGYPT_API ULimeTargetData : public UActiveTargetData
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	EChargingState ChargingState = EChargingState::Idle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CurrentCharge = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxCharge = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ChargeRate = 0.33;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DepleteRate = 0.33;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DepleteOverTimeRate = 0.33;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDepletesOverTime = false;
};
