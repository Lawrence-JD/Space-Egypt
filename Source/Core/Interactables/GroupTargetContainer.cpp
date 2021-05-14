// Fill out your copyright notice in the Description page of Project Settings.


#include "GroupTargetContainer.h"

// Sets default values
AGroupTargetContainer::AGroupTargetContainer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CyanTarget = CreateDefaultSubobject<UCyanGroupTarget>(TEXT("CyanGroupTarget"));
	YellowTarget = CreateDefaultSubobject<UYellowGroupTarget>(TEXT("YellowGroupTarget"));
}

// Called when the game starts or when spawned
void AGroupTargetContainer::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AGroupTargetContainer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

