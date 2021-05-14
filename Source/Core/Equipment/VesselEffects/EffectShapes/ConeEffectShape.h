// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EffectShape.h"
#include "Core/Equipment/VesselEffects/EffectShapes/EffectShape.h"
#include "ConeEffectShape.generated.h"

/**
 * 
 */
UCLASS()
class SPEEGYPT_API UConeEffectShape : public UEffectShape
{
public:

	GENERATED_BODY()

	UConeEffectShape();

	//virtual void BeginPlay() override;

	//void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void SetupMesh() override;

	virtual void UpdateDecalVectors() override;
};
