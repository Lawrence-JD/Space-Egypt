// Fill out your copyright notice in the Description page of Project Settings.


#include "OrangeVesselTarget.h"

// Sets default values for this component's properties
UOrangeVesselTarget::UOrangeVesselTarget()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	Init();
}

void UOrangeVesselTarget::Init()
{
	DataEntryPrefix = "OrangeData_";
	DataType = UOrangeTargetData::StaticClass();
	CollisionChannel = ORANGE_CHANNEL;
	Super::Init();
}

// Called when the game starts
void UOrangeVesselTarget::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UOrangeVesselTarget::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

