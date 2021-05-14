// Fill out your copyright notice in the Description page of Project Settings.


#include "CyanGroupTarget.h"
#include "../../TargetData/CyanTargetData.h"

// Sets default values for this component's properties
UCyanGroupTarget::UCyanGroupTarget()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	Init();
}

void UCyanGroupTarget::Init()
{
	DataEntryPrefix = "CyanGroupData_";
	DataType = UCyanTargetData::StaticClass();
	CollisionChannel = CYAN_CHANNEL;

	Initialize();

}

// Called when the game starts
void UCyanGroupTarget::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCyanGroupTarget::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
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
				for (auto& Name : PureComponent->GroupMembers)
				{
					UPrimitiveComponent* Primitive = GetComponentByName(Name);
					if (Primitive)
					{
						for (auto& Direction : PureComponent->ForceDirections)
						{
							//FVector ApexPoint = Direction->SourcePosition + (Direction->SourceHalfHeight * Direction->Direction);
							//FVector ApexToObject = Primitive->GetComponentLocation() - ApexPoint;
							//FVector ApexToSource = Direction->SourcePosition - ApexPoint;
							//float ObjectDotSource = FVector::DotProduct(ApexToObject, ApexToSource);
							//if (ObjectDotSource >= 0)
								Primitive->AddForce(Direction->SourceMagnitude * Direction->Direction);
						}
					}
				}
			}
		}
	}
}
