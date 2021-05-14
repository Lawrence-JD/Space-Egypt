// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PassiveVesselTarget.h"
#include "CyanVesselTarget.h"
#include "MagentaVesselTarget.h"
#include "../TargetData/YellowTargetData.h"
#include "YellowVesselTarget.generated.h"

// Used to handle interactions between an actors components and the yellow vessel effect.
// It will handle the collision profiles of components that are affected by the yellow vessel,
// as well as the visibility and physics simulation of affected components.
UCLASS( ClassGroup=(Custom), meta = (BlueprintSpawnableComponent))
class SPEEGYPT_API UYellowVesselTarget : public UPassiveVesselTarget
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UYellowVesselTarget();

	void Init() override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
		
};
