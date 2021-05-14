// Fill out your copyright notice in the Description page of Project Settings.


#include "SphereRoomba.h"

// Sets default values
ASphereRoomba::ASphereRoomba()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Vessel->SetEffectShapeType(EEffectShapeType::Sphere);
}

// Called when the game starts or when spawned
void ASphereRoomba::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASphereRoomba::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
