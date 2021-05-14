// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VesselEffects.h"
#include "EmptyVesselEffect.generated.h"

/// Empty class used to represent the empty vessel state.
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPEEGYPT_API UEmptyVesselEffect : public UVesselEffects
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEmptyVesselEffect();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
