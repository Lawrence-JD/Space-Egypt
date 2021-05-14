// Fill out your copyright notice in the Description page of Project Settings.


#include "CyanVesselTarget.h"
#include "../../../../HelperFiles/DefinedDebugHelpers.h"

// Sets default values for this component's properties
UCyanVesselTarget::UCyanVesselTarget()
{
	Init();
}

void UCyanVesselTarget::Init()
{
	DataEntryPrefix = "CyanData_";
	DataType = UCyanTargetData::StaticClass();
	CollisionChannel = CYAN_CHANNEL;

	Super::Init();
}

// Called when the game starts
void UCyanVesselTarget::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void UCyanVesselTarget::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
		
	for (auto& Component : PrimitiveComponents)
	{
		if (Component)
		{
			UCyanTargetData* PureComponent = Cast<UCyanTargetData>(Component);
			if (PureComponent && PureComponent->bApplyForce && PureComponent->bIsAffectedByThisEffect)
			{
				PureComponent->UpdateComponentDirection();
				UPrimitiveComponent* Primitive = GetComponentByName(PureComponent->ComponentName);
				if (Primitive)
				{
					for (auto& Direction : PureComponent->ForceDirections)
					{
					//	FVector ApexPoint = Direction->SourcePosition + (Direction->SourceHalfHeight * Direction->Direction);
					//	FVector ApexToObject = Primitive->GetComponentLocation() - ApexPoint;
					//	FVector ApexToSource = Direction->SourcePosition - ApexPoint;
					//	float ObjectDotSource = FVector::DotProduct(ApexToObject, ApexToSource);
					//	if (ObjectDotSource >= 0)
							Primitive->AddForce(Direction->SourceMagnitude * Direction->Direction);
					}
				}
			}
		}
	}
}
