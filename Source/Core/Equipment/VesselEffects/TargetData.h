// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetData.generated.h"

#define LIME_CHANNEL      ECC_GameTraceChannel1
#define INTERACT_CHANNEL  ECC_GameTraceChannel2
#define CYAN_CHANNEL      ECC_GameTraceChannel3
#define MAGENTA_CHANNEL   ECC_GameTraceChannel4
#define YELLOW_CHANNEL    ECC_GameTraceChannel5
#define VIOLET_CHANNEL    ECC_GameTraceChannel6
#define VIOLET_EFFECT 	  ECC_GameTraceChannel7
#define ORANGE_CHANNEL    ECC_GameTraceChannel8
#define FIND_SPOT_CHANNEL ECC_GameTraceChannel9

/// Parent class for all TargetData objects. Used to store the name of the component these settings apply to, as well as whether or not that component should be affected by this effect.
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPEEGYPT_API UTargetData : public UActorComponent
{
	GENERATED_BODY()

public:	

	/// String containing the name of the associated component. Using a pointer to that component caused problems, and would not work with blueprint actors.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString ComponentName = "";

	/// This determines if the component is affected by the current vessel effect, useful for actors who have multiple components/interact with multiple colors.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsAffectedByThisEffect = false;

};
