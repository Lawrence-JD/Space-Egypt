// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../PassiveVesselTarget.h"
#include "Components/ActorComponent.h"
#include "GroupVesselTarget.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPEEGYPT_API UGroupVesselTarget : public UPassiveVesselTarget
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGroupVesselTarget();

	void Initialize();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UTargetData* GetDataOfComponent(UPrimitiveComponent* Component) override;

	UTargetData* GetDataOfSceneComponent(USceneComponent* Component);

	USceneComponent* GetSceneComponentByName(FString Name);

#if WITH_EDITOR
	void RefreshComponentData() override;
#endif
};
