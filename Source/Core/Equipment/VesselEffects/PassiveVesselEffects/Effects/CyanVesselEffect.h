// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../VesselTargets/CyanVesselTarget.h"
#include "../Misc/CyanDirection.h"
#include "PassiveVesselEffect.h"
#include "CyanVesselEffect.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPEEGYPT_API UCyanVesselEffect : public UPassiveVesselEffect
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCyanVesselEffect();

	virtual void ApplyEffect(UPassiveVesselTarget* Target, UStaticMeshComponent* TargetComponent) override;

	virtual void RemoveEffect(UPassiveVesselTarget* Target, UStaticMeshComponent* TargetComponent) override;

	virtual void ApplyEffectToGroup(UPassiveVesselTarget* Target, UTargetData* Data) override;

	virtual	void RemoveEffectFromGroup(UPassiveVesselTarget* Target, UTargetData* Data) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ForceMagnitude = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCyanDirection* ForceDirection;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
