// Fill out your copyright notice in the Description page of Project Settings.


#include "OrangeVesselEffect.h"

// Sets default values for this component's properties
UOrangeVesselEffect::UOrangeVesselEffect()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	PillarManager = CreateDefaultSubobject<URockPillarManager>(TEXT("Pillar Manager"));
	Range = 1000;
	PillarManager->MaxPillars = 3;
	PillarManager->MaxPillarLifetime = 10;
}


void UOrangeVesselEffect::ApplyEffect()
{
	if (Cast<AActor>(HitResult.Actor))
	{
		UOrangeVesselTarget* Target = Cast<UOrangeVesselTarget>(HitResult.Actor->GetComponentByClass(UOrangeVesselTarget::StaticClass()));
		if (Target && Target->Owner)
		{
			UOrangeTargetData* Data = Cast<UOrangeTargetData>(Target->GetDataOfComponent(Cast<UPrimitiveComponent>(HitResult.Component)));
			if (PillarManager && Data && Data->bIsAffectedByThisEffect)
			{
				FVector Location = FVector(HitResult.Location.X, HitResult.Location.Y, HitResult.Location.Z);
				FRotator OtherUp = HitResult.Normal.Rotation();
				FRotator Rotation = FRotator(OtherUp.Pitch - 90, OtherUp.Yaw, OtherUp.Roll);
				Location += 80 * OtherUp.Vector();
				PillarManager->SpawnPillar(Location, Rotation, ChargeAmount, Cast<UStaticMeshComponent>(HitResult.GetComponent()));
			}
		}
	}
}

void UOrangeVesselEffect::DrawReticle()
{
	if (Cast<AActor>(HitResult.Actor))
	{
		UOrangeVesselTarget* Target = Cast<UOrangeVesselTarget>(HitResult.Actor->GetComponentByClass(UOrangeVesselTarget::StaticClass()));
		if (Target && Target->Owner)
		{
			UOrangeTargetData* Data = Cast<UOrangeTargetData>(Target->GetDataOfComponent(Cast<UPrimitiveComponent>(HitResult.Component)));
			if (PillarManager && Data && Data->bIsAffectedByThisEffect)
			{
				FVector Location = FVector(HitResult.Location.X, HitResult.Location.Y, HitResult.Location.Z);
				FRotator OtherUp = HitResult.Normal.Rotation();
				FRotator Rotation = FRotator(OtherUp.Pitch - 90, OtherUp.Yaw, OtherUp.Roll);
				Location += 80 * OtherUp.Vector();
				PillarManager->DrawReticle(Location, Rotation);
			}
		}
	}
}

void UOrangeVesselEffect::HideReticle()
{
	if(PillarManager)
		PillarManager->HideReticle();
}

// Called when the game starts
void UOrangeVesselEffect::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UOrangeVesselEffect::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

