// Fill out your copyright notice in the Description page of Project Settings.


#include "VesselEffects.h"

// Sets default values for this component's properties
UVesselEffects::UVesselEffects()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UVesselEffects::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UVesselEffects::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UVesselEffects::Enable()
{
	SetComponentTickEnabled(true);
}

void UVesselEffects::Disable()
{
	SetComponentTickEnabled(false);
}

void UVesselEffects::Aim()
{
	;
}

void UVesselEffects::Fire()
{
	;
}

void UVesselEffects::ResetAim()
{
	;
}

void UVesselEffects::ResetFire()
{
	;
}

bool UVesselEffects::IsActorAffected(AActor* Actor)
{
	if (Actor)
	{
		for (auto& Entry : Targets)
		{
			if (Entry && Entry->Owner)
			{
				if (Entry->Owner->GetName() == Actor->GetName())
					return true;
			}
		}
	}
	return false;
}

bool UVesselEffects::IsComponentAffected(UStaticMeshComponent* Component)
{
	if (Component && Component->GetOwner())
	{
		if (AffectedComponents.Contains(Component))
			return true;
		//for (int i = 0; i < Targets.Num(); i++)
		//{
		//	UVesselTarget* Entry = Targets[i];
		//	if (Entry && Entry->GetOwner() && Entry->Owner)
		//	{
		//		if (Entry->GetOwner()->GetName() != Component->GetOwner()->GetName())
		//			continue;
		//
		//		TArray<UPrimitiveComponent*> PrimitiveComponentFinder;
		//		Entry->Owner->GetComponents<UPrimitiveComponent>(PrimitiveComponentFinder);
		//
		//		if (PrimitiveComponentFinder.Num() > 0)
		//		{
		//			for (auto& Primitive : PrimitiveComponentFinder)
		//			{
		//				if (Primitive && Primitive->GetName() == Component->GetName())
		//					return true;
		//			}
		//		}
		//	}
		//}
	}
	return false;
}