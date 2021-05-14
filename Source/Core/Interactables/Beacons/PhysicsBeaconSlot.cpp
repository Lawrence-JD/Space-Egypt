// Fill out your copyright notice in the Description page of Project Settings.


#include "PhysicsBeaconSlot.h"
#include "PhysicsBeacon.h"

// Sets default values
APhysicsBeaconSlot::APhysicsBeaconSlot()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SlotMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SlotMesh"));

	RestingPoint = CreateDefaultSubobject<USceneComponent>(TEXT("RestingPoint"));
}

// Called when the game starts or when spawned
void APhysicsBeaconSlot::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APhysicsBeaconSlot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APhysicsBeaconSlot::TakePhysicsBeacon(APhysicsBeacon* BeaconToTake)
{
	if (BeaconToTake && !PhysicsBeacon)
	{
		EAttachmentRule RuleLocation = EAttachmentRule::SnapToTarget;
		EAttachmentRule RuleRotation = EAttachmentRule::SnapToTarget;
		EAttachmentRule RuleScale = EAttachmentRule::KeepWorld;
		FAttachmentTransformRules Rules = FAttachmentTransformRules(RuleLocation, RuleRotation, RuleScale, true);

		BeaconToTake->BeaconMesh->SetWorldScale3D(FVector(0.5, 0.5, 0.5));
		BeaconToTake->BeaconMesh->AttachToComponent(RestingPoint, Rules);
		PhysicsBeacon = BeaconToTake;
	}
}

void APhysicsBeaconSlot::OnInteract_Implementation(ASpeegyptCharacter* Player)
{
	if (Player && PhysicsBeacon)
	{
		if (!Player->HeldObject)
		{
			Player->GrabObject(PhysicsBeacon);
			PhysicsBeacon = nullptr;
		}
	}
}

