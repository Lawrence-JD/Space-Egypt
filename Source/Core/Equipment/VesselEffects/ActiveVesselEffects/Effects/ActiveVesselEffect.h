// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "../../VesselEffects.h"
#include "../../../VesselEnums.h"
#include "../Targets/ActiveVesselTarget.h"
#include "../TargetData/ActiveTargetData.h"
#include "../../../../Helperfiles/DefinedDebugHelpers.h"
#include "ActiveVesselEffect.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPEEGYPT_API UActiveVesselEffect : public UVesselEffects
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UActiveVesselEffect();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EVesselAbilityState AbilityState;

	void Disable() override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
