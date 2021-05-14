// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PassiveTargetData.h"
#include "MagentaTargetData.generated.h"


UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent), NonTransient)
class SPEEGYPT_API UMagentaTargetData : public UPassiveTargetData
{
	GENERATED_BODY()

public:	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsFixed = false;
		
};
