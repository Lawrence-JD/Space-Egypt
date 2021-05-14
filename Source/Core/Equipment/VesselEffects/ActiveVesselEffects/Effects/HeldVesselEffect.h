// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActiveVesselEffect.h"
#include "HeldVesselEffect.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPEEGYPT_API UHeldVesselEffect : public UActiveVesselEffect
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHeldVesselEffect();

	void Aim() override;

	void Fire() override;

	void ResetAim() override;

	void ResetFire() override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
