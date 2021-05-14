// Fill out your copyright notice in the Description page of Project Settings.


#include "LimeVesselTarget.h"
#include "../../../../HelperFiles/DefinedDebugHelpers.h"

// Sets default values for this component's properties
ULimeVesselTarget::ULimeVesselTarget()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	Init();
}

void ULimeVesselTarget::Init()
{
	DataEntryPrefix = "LimeData_";
	DataType = ULimeTargetData::StaticClass();
	CollisionChannel = LIME_CHANNEL;
	Super::Init();
}

// Called when the game starts
void ULimeVesselTarget::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void ULimeVesselTarget::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (auto& Component : PrimitiveComponents)
	{
		if (Component)
		{
			ULimeTargetData* PureComponent = Cast<ULimeTargetData>(Component);
			if (PureComponent)
			{
				switch (PureComponent->ChargingState)
				{
				case EChargingState::Idle:
					if (PureComponent->bDepletesOverTime)
					{
						if (PureComponent->CurrentCharge > 0)
						{
							PureComponent->CurrentCharge -= PureComponent->DepleteOverTimeRate * DeltaTime;
						}
						if (PureComponent->CurrentCharge <= 0)
						{
							PureComponent->CurrentCharge = 0;
						}
					}
					break;
				case EChargingState::Charging:
					if (PureComponent->CurrentCharge < PureComponent->MaxCharge)
					{
						PureComponent->CurrentCharge += PureComponent->ChargeRate * DeltaTime;
					}
					if (PureComponent->CurrentCharge >= PureComponent->MaxCharge)
					{
						PureComponent->CurrentCharge = PureComponent->MaxCharge;
					}
					break;
				case EChargingState::Depleteing:
					if (PureComponent->CurrentCharge > 0)
					{
						PureComponent->CurrentCharge -= PureComponent->DepleteRate * DeltaTime;
					}
					if (PureComponent->CurrentCharge <= 0)
					{
						PureComponent->CurrentCharge = 0;
					}
					break;
				}
				//SCREENMSGF("Charge: ", PureComponent->CurrentCharge);
			}
		}
	}
}

