// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interactable.h"
#include "../../Equipment/VesselEnums.h"
#include "../../Equipment/Vessel.h"
#include "Beacon.generated.h"


UCLASS(ABSTRACT)
class SPEEGYPT_API ABeacon : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABeacon();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* BeaconMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UVessel* Vessel;

#if WITH_EDITOR
	virtual bool ShouldTickIfViewportsOnly() const override;
#endif

	virtual void OnInteract_Implementation(ASpeegyptCharacter* Player) override;

protected:

	//void PostInitializeComponents() override;

	UFUNCTION()
	void OnVesselColorUpdate();

	virtual void BeginPlay() override;

	//UPROPERTY()
	//bool bMatFlag = true;

	//UPROPERTY()
	//bool bSetupFlag = false;

	//UPROPERTY()
	//EVesselState StartupState;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
