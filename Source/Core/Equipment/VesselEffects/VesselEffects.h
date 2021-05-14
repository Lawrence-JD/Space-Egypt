// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VesselTarget.h"
#include "../VesselEnums.h"
#include "VesselEffects.generated.h"

#define LIME_CHANNEL     ECC_GameTraceChannel1
#define INTERACT_CHANNEL ECC_GameTraceChannel2
#define CYAN_CHANNEL     ECC_GameTraceChannel3
#define MAGENTA_CHANNEL  ECC_GameTraceChannel4
#define YELLOW_CHANNEL   ECC_GameTraceChannel5
#define VIOLET_CHANNEL   ECC_GameTraceChannel6
#define ORANGE_CHANNEL   ECC_GameTraceChannel8

/// Parent class of all Vessel Effects. Used to keep track of the current targets of the effect, as well as enabling/disabling the ticking of the effect based on whether it is the current effect or not.
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPEEGYPT_API UVesselEffects : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties

	UVesselEffects();

	/// Array of all Vessel Targets currently being affected by this effect. Mainly used in the passive effects, as well as Lime.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UVesselTarget*> Targets;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TSet<UStaticMeshComponent*> AffectedComponents;

	/// Enables ticking for this effect.
	UFUNCTION()
	virtual void Enable();

	/// Disables ticking for this effect.
	UFUNCTION()
	virtual void Disable();

	/// Overridden by derived classes. Ensures aiming and firing while using a passive ability does nothing.
	UFUNCTION()
	virtual void Aim();

	/// Overridden by derived classes. Ensures aiming and firing while using a passive ability does nothing.
	UFUNCTION()
	virtual void Fire();

	/// Overridden by derived classes. Ensures aiming and firing while using a passive ability does nothing.
	UFUNCTION()
	virtual void ResetAim();

	/// Overridden by derived classes. Ensures aiming and firing while using a passive ability does nothing.
	UFUNCTION()
	virtual void ResetFire();

	/// Helper function used to determine if the given actor is affected by this effect.
	UFUNCTION(BlueprintCallable)
	bool IsActorAffected(AActor* Actor);

	/// Helper function used to determine if the given component is affected by this effect.
	UFUNCTION(BlueprintCallable)
	bool IsComponentAffected(UStaticMeshComponent* Component);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
