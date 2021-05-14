// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PassiveVesselEffect.h"
#include "../VesselTargets/MagentaVesselTarget.h"
#include "MagentaVesselEffect.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPEEGYPT_API UMagentaVesselEffect : public UPassiveVesselEffect
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMagentaVesselEffect();

	virtual void ApplyEffect(UPassiveVesselTarget* Target, UStaticMeshComponent* TargetComponent) override;

	virtual void RemoveEffect(UPassiveVesselTarget* Target, UStaticMeshComponent* TargetComponent) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
