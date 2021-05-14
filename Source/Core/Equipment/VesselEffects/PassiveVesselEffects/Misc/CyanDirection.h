// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CyanDirection.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPEEGYPT_API UCyanDirection : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCyanDirection();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector Direction;

	UPROPERTY()
	FVector SourcePosition;

	UPROPERTY()
	float SourceMagnitude = 0;

	UFUNCTION()
	void CalculateForceDirection();
};
