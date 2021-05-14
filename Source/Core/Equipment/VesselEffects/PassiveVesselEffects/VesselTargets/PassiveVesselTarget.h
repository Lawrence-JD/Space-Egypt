// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../../VesselTarget.h"
#include "../TargetData/PassiveTargetData.h"
#include "PassiveVesselTarget.generated.h"


UCLASS(ABSTRACT)
class SPEEGYPT_API UPassiveVesselTarget : public UVesselTarget
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPassiveVesselTarget();

	// Used to add to the number of sources affecting the given PrimitiveComponent. Returns true if successful.
	UFUNCTION()
	virtual bool AddSource(UPrimitiveComponent* Component);

	// Used to remove from the number of sources affecting the given PrimitiveComponent. Returns 0 if unsuccessful, 1 if successful and Num Sources > 0, 2 if Num Sources == 0. Will raise and exception if Num Sources < 0.
	UFUNCTION()
	virtual int RemoveSource(UPrimitiveComponent* Component);

	bool AddSource(UPassiveTargetData* Data);

	int RemoveSource(UPassiveTargetData* Data);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Resets Num Sources of all components to 0.
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// called every frame
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);

};
