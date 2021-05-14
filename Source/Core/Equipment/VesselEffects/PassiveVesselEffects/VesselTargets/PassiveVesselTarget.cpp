// Fill out your copyright notice in the Description page of Project Settings.


#include "PassiveVesselTarget.h"
#include "CyanVesselTarget.h"
#include "MagentaVesselTarget.h"
#include "YellowVesselTarget.h"

// Sets default values for this component's properties
UPassiveVesselTarget::UPassiveVesselTarget()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UPassiveVesselTarget::BeginPlay()
{
	Super::BeginPlay();

}

void UPassiveVesselTarget::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (auto& Component : PrimitiveComponents)
	{
		if (Component)
		{
			UPassiveTargetData* PureComponent = Cast<UPassiveTargetData>(Component);
			if(PureComponent)
				PureComponent->NumSources = 0;
		}
	}
}

void UPassiveVesselTarget::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

bool UPassiveVesselTarget::AddSource(UPrimitiveComponent* Component)
{
	UPassiveTargetData* NewData = Cast<UPassiveTargetData>(GetDataOfComponent(Component));

	return AddSource(NewData);
}

int UPassiveVesselTarget::RemoveSource(UPrimitiveComponent* Component)
{
	UPassiveTargetData* NewData = Cast<UPassiveTargetData>(GetDataOfComponent(Component));

	return RemoveSource(NewData);
}

bool UPassiveVesselTarget::AddSource(UPassiveTargetData* Data)
{
	if (Data)
	{
		Data->NumSources++;
		return true;
	}
	return false;
}

int UPassiveVesselTarget::RemoveSource(UPassiveTargetData* Data)
{
	if (Data)
	{
		if (Data->NumSources <= 0)
			UE_LOG(LogTemp, Warning, TEXT("Trying to remove a source from component that has no source"));
		
		Data->NumSources--;

		if (!Data->NumSources)
			return 2;

		return 1;
	}
	return 0;
}