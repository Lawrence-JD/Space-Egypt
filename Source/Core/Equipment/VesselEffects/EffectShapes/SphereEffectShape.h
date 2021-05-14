// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EffectShape.h"
#include "Core/Equipment/VesselEffects/EffectShapes/EffectShape.h"
#include "SphereEffectShape.generated.h"

/**
 * 
 */
UCLASS()
class SPEEGYPT_API USphereEffectShape : public UEffectShape
{
public:

	GENERATED_BODY()

	USphereEffectShape();

	virtual void SetupMesh() override;

	virtual void UpdateDecalVectors() override;
};
