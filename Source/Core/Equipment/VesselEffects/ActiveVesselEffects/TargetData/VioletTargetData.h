// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveTargetData.h"
#include "VioletTargetData.generated.h"


UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent), NonTransient)
class SPEEGYPT_API UVioletTargetData : public UActiveTargetData
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UMaterialInterface* BaseMat;


};
