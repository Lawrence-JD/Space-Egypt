// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../../TargetData.h"
#include "PassiveTargetData.generated.h"

UCLASS()
class SPEEGYPT_API UPassiveTargetData : public UTargetData
{
	GENERATED_BODY()

public:	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient)
	int NumSources = 0;

	UPROPERTY(VisibleAnywhere)
	TArray<FString> GroupMembers;
};