// Fill out your copyright notice in the Description page of Project Settings.


#include "MagentaVesselTarget.h"

// Sets default values for this component's properties
UMagentaVesselTarget::UMagentaVesselTarget()
{
	Init();
}

void UMagentaVesselTarget::Init()
{
	DataEntryPrefix = "MagentaData_";
	DataType = UMagentaTargetData::StaticClass();
	CollisionChannel = MAGENTA_CHANNEL;

	Super::Init();
}

// Called when the game starts
void UMagentaVesselTarget::BeginPlay()
{
	Super::BeginPlay();

}
