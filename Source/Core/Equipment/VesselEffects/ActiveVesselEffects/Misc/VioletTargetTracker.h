// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Targets/VioletVesselTarget.h"
#include "VioletTargetTracker.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPEEGYPT_API UVioletTargetTracker : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVioletTargetTracker();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UVioletVesselTarget*> VioletTargets;

	UFUNCTION(BlueprintCallable)
	void UpdateTargets();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;		
};
