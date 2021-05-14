// Fill out your copyright notice in the Description page of Project Settings.


#include "VioletTeleportTarget.h"

// Sets default values for this component's properties
UVioletTeleportTarget::UVioletTeleportTarget()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	Init();
}

void UVioletTeleportTarget::Init()
{
	DataEntryPrefix = "VioletTeleportData_";
	DataType = UVioletTeleportData::StaticClass();
	CollisionChannel = VIOLET_EFFECT;
	Super::Init();
}

// Called when the game starts
void UVioletTeleportTarget::BeginPlay()
{
	Super::BeginPlay();

	if (Owner)
	{
		for (auto& Component : PrimitiveComponents)
		{
			if (Component)
			{
				UVioletTeleportData* PureComponent = Cast<UVioletTeleportData>(Component);
				if (PureComponent && PureComponent->bIsAffectedByThisEffect)
				{
					UPrimitiveComponent* Primitive = GetComponentByName(PureComponent->ComponentName);
					if (Primitive)
					{
						Primitive->SetMobility(EComponentMobility::Movable);
						Primitive->SetSimulatePhysics(true);
					}
				}
			}
		}
	}
}


// Called every frame
void UVioletTeleportTarget::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

