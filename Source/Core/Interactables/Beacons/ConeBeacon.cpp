// Fill out your copyright notice in the Description page of Project Settings.


#include "ConeBeacon.h"

// Sets default values
AConeBeacon::AConeBeacon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Vessel->SetEffectShapeType(EEffectShapeType::Cone);
}

// Called when the game starts or when spawned
void AConeBeacon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AConeBeacon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

