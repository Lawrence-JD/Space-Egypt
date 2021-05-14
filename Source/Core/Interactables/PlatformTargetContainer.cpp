// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformTargetContainer.h"

// Sets default values
APlatformTargetContainer::APlatformTargetContainer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	LimeTarget = CreateDefaultSubobject<ULimeVesselTarget>(TEXT("LimeTarget"));
	MagentaTarget = CreateDefaultSubobject<UMagentaVesselTarget>(TEXT("MagentaTarget"));
	YellowTarget = CreateDefaultSubobject<UYellowVesselTarget>(TEXT("YellowTarget"));
	OrangeTarget = CreateDefaultSubobject<UOrangeVesselTarget>(TEXT("OrangeTarget"));

}

// Called when the game starts or when spawned
void APlatformTargetContainer::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APlatformTargetContainer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

