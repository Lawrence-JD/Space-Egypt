// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SpeegyptHUD.generated.h"

UCLASS()
class ASpeegyptHUD : public AHUD
{
	GENERATED_BODY()

public:
	ASpeegyptHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

