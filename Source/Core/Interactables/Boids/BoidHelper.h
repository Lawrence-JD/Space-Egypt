// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "BoidHelper.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPEEGYPT_API UBoidHelper : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBoidHelper();

	UPROPERTY()
	int NumViewDirections = 300;
	
	UPROPERTY()
	TArray<FVector> Directions;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
		
};
