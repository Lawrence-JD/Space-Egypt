// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveVesselTarget.h"
#include "../TargetData/VioletTeleportData.h"
#include "VioletTeleportTarget.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPEEGYPT_API UVioletTeleportTarget : public UActiveVesselTarget
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVioletTeleportTarget();

	void Init() override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
