// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GroupVesselTarget.h"
#include "Components/ActorComponent.h"
#include "YellowGroupTarget.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPEEGYPT_API UYellowGroupTarget : public UGroupVesselTarget
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UYellowGroupTarget();

	void Init() override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
