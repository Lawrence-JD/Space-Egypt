// Fill out your copyright notice in the Description page of Project Settings.


#include "BoxBeacon.h"

// Sets default values
ABoxBeacon::ABoxBeacon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Vessel->SetEffectShapeType(EEffectShapeType::Box);
}

// Called when the game starts or when spawned
void ABoxBeacon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABoxBeacon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

