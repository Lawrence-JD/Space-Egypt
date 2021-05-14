// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveTargetData.h"
#include "CollisionShape.h"
#include "VioletTeleportData.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPEEGYPT_API UVioletTeleportData : public UActiveTargetData
{
	GENERATED_BODY()

public:	

	bool temp;
		
};
