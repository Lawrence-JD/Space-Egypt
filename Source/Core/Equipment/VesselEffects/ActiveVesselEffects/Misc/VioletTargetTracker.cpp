// Fill out your copyright notice in the Description page of Project Settings.


#include "VioletTargetTracker.h"

// Sets default values for this component's properties
UVioletTargetTracker::UVioletTargetTracker()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UVioletTargetTracker::BeginPlay()
{
	Super::BeginPlay();

	UpdateTargets();
	
}

void UVioletTargetTracker::UpdateTargets()
{
	UWorld* World = GetWorld();
	if (World)
	{
		ULevel* Level = World->GetCurrentLevel();
		if (Level)
		{
			for (auto& Actor : Level->Actors)
			{
				if (Actor)
				{
					UActorComponent* TargetToAdd = Actor->GetComponentByClass(UVioletVesselTarget::StaticClass());
					if (TargetToAdd)
					{
						UVioletVesselTarget* PureTargetToAdd = Cast<UVioletVesselTarget>(TargetToAdd);
						if (PureTargetToAdd)
							VioletTargets.Add(PureTargetToAdd);
					}
				}
			}
		}
	}
}
