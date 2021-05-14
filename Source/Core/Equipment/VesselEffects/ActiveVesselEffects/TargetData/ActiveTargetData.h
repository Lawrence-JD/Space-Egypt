#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../../TargetData.h"
#include "ActiveTargetData.generated.h"

UCLASS(ABSTRACT)
class SPEEGYPT_API UActiveTargetData : public UTargetData
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient)
	bool active;

};