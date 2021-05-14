// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "../Character/SpeegyptCharacter.h"
#include "../Equipment/VesselEffects/PassiveVesselEffects/VesselTargets/YellowVesselTarget.h"
#include "Interactable.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

class SPEEGYPT_API IInteractable
{
	GENERATED_BODY()
	
public:	

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnInteract(ASpeegyptCharacter* Player);

	virtual void OnInteract_Implementation(ASpeegyptCharacter* Player) { ; };

};
