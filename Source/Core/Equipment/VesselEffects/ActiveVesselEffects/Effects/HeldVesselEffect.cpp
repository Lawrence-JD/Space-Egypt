// Fill out your copyright notice in the Description page of Project Settings.


#include "HeldVesselEffect.h"

// Sets default values for this component's properties
UHeldVesselEffect::UHeldVesselEffect()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UHeldVesselEffect::Aim()
{
	;
}

void UHeldVesselEffect::Fire()
{
	;
}

void UHeldVesselEffect::ResetAim()
{
	;
}

void UHeldVesselEffect::ResetFire()
{
	;
}

// Called when the game starts
void UHeldVesselEffect::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UHeldVesselEffect::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

