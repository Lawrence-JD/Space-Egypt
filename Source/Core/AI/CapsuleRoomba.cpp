// Fill out your copyright notice in the Description page of Project Settings.


#include "CapsuleRoomba.h"

// Sets default values
ACapsuleRoomba::ACapsuleRoomba()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Vessel->SetEffectShapeType(EEffectShapeType::Capsule);
}

// Called when the game starts or when spawned
void ACapsuleRoomba::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACapsuleRoomba::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

