// Fill out your copyright notice in the Description page of Project Settings.


#include "CapsuleBeacon.h"

// Sets default values
ACapsuleBeacon::ACapsuleBeacon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Vessel->SetEffectShapeType(EEffectShapeType::Capsule);
}

// Called when the game starts or when spawned
void ACapsuleBeacon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACapsuleBeacon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

