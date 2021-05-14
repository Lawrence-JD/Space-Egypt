// Fill out your copyright notice in the Description page of Project Settings.


#include "BoidSettings.h"

// Sets default values for this component's properties
UBoidSettings::UBoidSettings()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

    // Settings
    MinSpeed = 100;
    MaxSpeed = 300;
    PerceptionRadius = 1000;
    AvoidanceRadius = 200;
    MaxSteerForce = 3;

    AlignWeight = 11;
    CohesionWeight = 10;
    SeperateWeight = 8;

    TargetWeight = 1;

    BoundsRadius = 50;
    AvoidCollisionWeight = 80;
    CollisionAvoidDst = 1000;

	BoidHalfFOV = 90;
	BoidHalfFOVRads = BoidHalfFOV * (PI / 180);

	// ...
}


// Called when the game starts
void UBoidSettings::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}
