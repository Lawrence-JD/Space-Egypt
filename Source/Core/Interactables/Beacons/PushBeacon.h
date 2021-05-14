// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicsBeacon.h"
#include "GameFramework/Actor.h"
#include "PushBeacon.generated.h"

UCLASS()
class SPEEGYPT_API APushBeacon : public APhysicsBeacon
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APushBeacon() { PrimaryActorTick.bCanEverTick = true; };

	virtual void OnConstruction(const FTransform& Transform) override { Super::OnConstruction(Transform); BeaconMesh->SetWorldScale3D(FVector(0.75)); };

protected:

	virtual bool OnGrabBegin(ASpeegyptCharacter* Player) override { return false; };

	virtual bool OnGrabEnd(ASpeegyptCharacter* Player) override { return false; };

};
