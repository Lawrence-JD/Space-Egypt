// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Character/SpeegyptCharacter.h"
#include "UObject/Interface.h"
#include "GrabbableObject.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGrabbableObject : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SPEEGYPT_API IGrabbableObject
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION()
	virtual bool OnGrabBegin(ASpeegyptCharacter* Player) { return false; };

	UFUNCTION()
	virtual bool OnGrabEnd(ASpeegyptCharacter* Player) { return false; };
};
