// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "BoidSettings.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPEEGYPT_API UBoidSettings : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBoidSettings();

    // Settings
    UPROPERTY(EditAnywhere)
    float MinSpeed;

	UPROPERTY(EditAnywhere)
	float MaxSpeed;

	UPROPERTY(EditAnywhere)
	float PerceptionRadius;

	UPROPERTY(EditAnywhere)
	float AvoidanceRadius;

	UPROPERTY(EditAnywhere)
	float MaxSteerForce;

	UPROPERTY(EditAnywhere)
	float AlignWeight;

	UPROPERTY(EditAnywhere)
	float CohesionWeight;

	UPROPERTY(EditAnywhere)
	float SeperateWeight;

	UPROPERTY(EditAnywhere)
	float TargetWeight;

	UPROPERTY(EditAnywhere)
	float BoundsRadius ;

	UPROPERTY(EditAnywhere)
	float AvoidCollisionWeight;

	UPROPERTY(EditAnywhere)
	float CollisionAvoidDst;
	
	UPROPERTY(EditAnywhere)
	float BoidHalfFOV;

	UPROPERTY()
	float BoidHalfFOVRads;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;	
};
