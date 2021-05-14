// Fill out your copyright notice in the Description page of Project Settings.


#include "AllPurposeTargetContainer.h"

// Sets default values
AAllPurposeTargetContainer::AAllPurposeTargetContainer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CyanTarget = CreateDefaultSubobject<UCyanVesselTarget>(TEXT("CyanTarget"));
	MagentaTarget = CreateDefaultSubobject<UMagentaVesselTarget>(TEXT("MagentaTarget"));
	YellowTarget = CreateDefaultSubobject<UYellowVesselTarget>(TEXT("YellowTarget"));
	LimeTarget = CreateDefaultSubobject<ULimeVesselTarget>(TEXT("LimeTarget"));
	VioletTarget = CreateDefaultSubobject<UVioletVesselTarget>(TEXT("VioletTarget"));
	OrangeTarget = CreateDefaultSubobject<UOrangeVesselTarget>(TEXT("OrangeTarget"));

}

// Called when the game starts or when spawned
void AAllPurposeTargetContainer::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AAllPurposeTargetContainer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

