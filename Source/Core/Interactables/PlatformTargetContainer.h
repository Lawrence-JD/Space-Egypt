// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Equipment/VesselEffects/ActiveVesselEffects/Targets/LimeVesselTarget.h"
#include "../Equipment/VesselEffects/PassiveVesselEffects/VesselTargets/MagentaVesselTarget.h"
#include "../Equipment/VesselEffects/PassiveVesselEffects/VesselTargets/YellowVesselTarget.h"
#include "../Equipment/VesselEffects/ActiveVesselEffects/Targets/OrangeVesselTarget.h"
#include "PlatformTargetContainer.generated.h"

UCLASS()
class SPEEGYPT_API APlatformTargetContainer : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APlatformTargetContainer();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ULimeVesselTarget* LimeTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UMagentaVesselTarget* MagentaTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UYellowVesselTarget* YellowTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UOrangeVesselTarget* OrangeTarget;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
