// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/CapsuleComponent.h"
#include "ActiveVesselTarget.h"
#include "VioletVesselTarget.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPEEGYPT_API UVioletVesselTarget : public UActiveVesselTarget
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVioletVesselTarget();

	void Init() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCapsuleComponent* CaptureCapsule;

	UPROPERTY()
	UMaterial* RetMat;

	UPROPERTY()
		USceneComponent* PlayerSpawnLocation;
	void DrawReticle();

	void HideReticle();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
