// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PassiveTargetData.h"
#include "../Misc/CyanDirection.h"
#include "CyanTargetData.generated.h"


UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent), NonTransient)
class SPEEGYPT_API UCyanTargetData : public UPassiveTargetData
{
	GENERATED_BODY()

public:	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bApplyForce = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float ForceMagnitude = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector CurrentDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TSet<UCyanDirection*> ForceDirections;

	FORCEINLINE void UpdateComponentDirection()
	{
		FVector DirectionSum = FVector(0, 0, 0);
		for (auto& Vector : ForceDirections)
		{
			DirectionSum += Vector->Direction;
		}
		DirectionSum.Normalize();
		CurrentDirection = DirectionSum;
	};
};
