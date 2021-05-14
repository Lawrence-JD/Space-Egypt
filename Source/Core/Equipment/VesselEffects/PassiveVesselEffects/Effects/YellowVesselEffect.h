// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PassiveVesselEffect.h"
#include "../VesselTargets/YellowVesselTarget.h"
#include "../VesselTargets/GroupTargets/YellowGroupTarget.h"
#include "YellowVesselEffect.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPEEGYPT_API UYellowVesselEffect : public UPassiveVesselEffect
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UYellowVesselEffect();

	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void ApplyEffect(UPassiveVesselTarget* Target, UStaticMeshComponent* TargetComponent) override;

	virtual void RemoveEffect(UPassiveVesselTarget* Target, UStaticMeshComponent* TargetComponent) override;

	virtual void ApplyEffectToGroup(UPassiveVesselTarget* Target, UTargetData* Data) override;

	virtual void RemoveEffectFromGroup(UPassiveVesselTarget* Target, UTargetData* Data) override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
