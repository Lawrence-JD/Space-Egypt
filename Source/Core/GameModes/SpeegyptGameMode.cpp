// Copyright Epic Games, Inc. All Rights Reserved.

#include "SpeegyptGameMode.h"
#include "../UI/SpeegyptHUD.h"
#include "../Character/SpeegyptCharacter.h"
#include "SpeegyptPlayerController.h"
#include "UObject/ConstructorHelpers.h"

ASpeegyptGameMode::ASpeegyptGameMode()
	: Super()
{

	// set default pawn class to our Blueprinted character
	DefaultPawnClass = ASpeegyptCharacter::StaticClass();

	PlayerControllerClass = ASpeegyptPlayerController::StaticClass();

	// use our custom HUD class
	HUDClass = ASpeegyptHUD::StaticClass();
}
