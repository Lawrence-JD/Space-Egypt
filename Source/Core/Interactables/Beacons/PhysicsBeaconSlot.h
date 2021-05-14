// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Interactable.h"
#include "GameFramework/Actor.h"
#include "PhysicsBeaconSlot.generated.h"

class APhysicsBeacon;

UCLASS()
class SPEEGYPT_API APhysicsBeaconSlot : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APhysicsBeaconSlot();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* RestingPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* SlotMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	APhysicsBeacon* PhysicsBeacon;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void TakePhysicsBeacon(APhysicsBeacon* BeaconToTake);

	virtual void OnInteract_Implementation(ASpeegyptCharacter* Player) override;

	UFUNCTION(BlueprintCallable)
	APhysicsBeacon* GetAttachedBeacon() { return PhysicsBeacon; }
};
