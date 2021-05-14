// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AimedVesselEffect.h"
#include "../Misc/RockPillarManager.h"
#include "../Targets/OrangeVesselTarget.h"
#include "OrangeVesselEffect.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPEEGYPT_API UOrangeVesselEffect : public UAimedVesselEffect
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOrangeVesselEffect();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	URockPillarManager* PillarManager;

	virtual void ApplyEffect() override;

	virtual void DrawReticle() override;

	virtual void HideReticle() override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
