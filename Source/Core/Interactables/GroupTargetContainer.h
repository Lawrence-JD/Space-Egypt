// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Equipment/VesselEffects/PassiveVesselEffects/VesselTargets/GroupTargets/CyanGroupTarget.h"
#include "../Equipment/VesselEffects/PassiveVesselEffects/VesselTargets/GroupTargets/YellowGroupTarget.h"


#include "GroupTargetContainer.generated.h"

UCLASS()
class SPEEGYPT_API AGroupTargetContainer : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGroupTargetContainer();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UYellowGroupTarget* YellowTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UCyanGroupTarget* CyanTarget;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
