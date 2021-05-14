// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AimedVesselEffect.h"
#include "../Targets/VioletVesselTarget.h"
#include "../Misc/VioletTargetTracker.h"
#include "Components/CapsuleComponent.h"
#include "VioletVesselEffect.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPEEGYPT_API UVioletVesselEffect : public UAimedVesselEffect
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVioletVesselEffect();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCapsuleComponent* CaptureCapsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UVioletTargetTracker* VioletTargetTracker;

	void Enable() override;

	void Disable() override;

	void DrawReticle() override;

	void HideReticle() override;

	void ApplyEffect() override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
