// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GroupVesselTarget.h"
#include "Components/ActorComponent.h"
#include "CyanGroupTarget.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPEEGYPT_API UCyanGroupTarget : public UGroupVesselTarget
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCyanGroupTarget();

	void Init() override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
