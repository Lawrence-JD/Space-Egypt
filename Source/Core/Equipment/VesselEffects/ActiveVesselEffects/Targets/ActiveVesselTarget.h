// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../../VesselTarget.h"
#include "../TargetData/ActiveTargetData.h"
#include "../TargetData/LimeTargetData.h"
#include "../TargetData/OrangeTargetData.h"
#include "../TargetData/VioletTargetData.h"
#include "ActiveVesselTarget.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPEEGYPT_API UActiveVesselTarget : public UVesselTarget
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UActiveVesselTarget();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
