// Fill out your copyright notice in the Description page of Project Settings.


#include "BoidHelper.h"

// Sets default values for this component's properties
UBoidHelper::UBoidHelper()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	float GoldenRatio = (1 + FMath::Sqrt(5)) / 2;
	float angleIncrement = PI * 2 * GoldenRatio;

	for (int i = 0; i < NumViewDirections; i++)
	{
		float t = (float)i / NumViewDirections;
		float inclination = FMath::Acos(1 - 2 * t);
		float azimuth = angleIncrement * i;

		float x = FMath::Sin(inclination) * FMath::Cos(azimuth);
		float y = FMath::Sin(inclination) * FMath::Sin(azimuth);
		float z = FMath::Cos(inclination);
		Directions.Add(FVector(x, y, z));
	}
}


// Called when the game starts
void UBoidHelper::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

