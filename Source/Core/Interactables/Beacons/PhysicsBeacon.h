// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Beacon.h"
#include "../GrabbableObject.h"
#include "PhysicsBeacon.generated.h"

UCLASS()
class SPEEGYPT_API APhysicsBeacon : public ABeacon, public IGrabbableObject
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APhysicsBeacon();

	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	bool bDynamicFlag = true;

	UPROPERTY()
	float FallingTimer = 0;

	virtual bool OnGrabBegin(ASpeegyptCharacter* Player) override;

	virtual bool OnGrabEnd(ASpeegyptCharacter* Player) override;

#if WITH_EDITOR
	virtual bool ShouldTickIfViewportsOnly() const override;
#endif

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
