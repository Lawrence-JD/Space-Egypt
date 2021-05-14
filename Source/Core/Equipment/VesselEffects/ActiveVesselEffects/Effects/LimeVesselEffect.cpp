// Fill out your copyright notice in the Description page of Project Settings.


#include "LimeVesselEffect.h"

// Sets default values for this component's properties
ULimeVesselEffect::ULimeVesselEffect()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;


	// Mesh Setup
	HitCone = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HitCone"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> VesselMeshFinder(TEXT("StaticMesh'/Game/Speegypt/Character/Mesh/LimeCone.LimeCone'"));
	if (VesselMeshFinder.Object)
	{
		HitCone->SetStaticMesh(VesselMeshFinder.Object);
		HitCone->SetWorldScale3D(FVector(8, 8, 5));
		HitCone->SetCollisionProfileName("LimeVessel");

		// Material Setup
		static ConstructorHelpers::FObjectFinder<UMaterial> VesselMatFinder(TEXT("Material'/Game/Speegypt/Effects/Materials/LimeConeMat.LimeConeMat'"));
		if (VesselMatFinder.Object)
		{
			HitCone->SetMaterial(0, VesselMatFinder.Object);
		}
		DisableHitCone();
	}


}

// Called when the game starts
void ULimeVesselEffect::BeginPlay()
{
	Super::BeginPlay();

	if(HitCone)
		HitCone->OnComponentEndOverlap.AddDynamic(this, &ULimeVesselEffect::OnOverlapEnd);
}

// Called every frame
void ULimeVesselEffect::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TArray<UPrimitiveComponent*> NewTargets;
	if(HitCone)
		HitCone->GetOverlappingComponents(NewTargets);
	for (auto& Target : NewTargets)
	{
		if (Target)
		{
			UStaticMeshComponent* NewTarget = Cast<UStaticMeshComponent>(Target);
			if (NewTarget && !IsComponentAffected(NewTarget))
			{
				TArray<UActorComponent*> VesselTargets;
				NewTarget->GetOwner()->GetComponents(VesselTargets);

				if (VesselTargets.Num() > 0)
				{
					for (auto& VesselTarget : VesselTargets)
					{
						ULimeVesselTarget* PureTarget = Cast<ULimeVesselTarget>(VesselTarget);
						if (PureTarget)
						{
							if (!Targets.Contains(PureTarget))
								Targets.Add(PureTarget);
							if (!AffectedComponents.Contains(NewTarget))
								AffectedComponents.Add(NewTarget);
							ApplyEffect(PureTarget, NewTarget);
						}
					}
				}
			}
		}
	}
}

void ULimeVesselEffect::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherComp)
	{
		UStaticMeshComponent* PureOtherComp = Cast<UStaticMeshComponent>(OtherComp);
		if (IsComponentAffected(PureOtherComp))
		{
			TArray<UActorComponent*> VesselTargets;
			OtherActor->GetComponents(VesselTargets);

			if (VesselTargets.Num() > 0)
			{
				for (auto& Target : VesselTargets)
				{
					if (Target)
					{
						ULimeVesselTarget* PureTarget = Cast<ULimeVesselTarget>(Target);
						if (PureTarget)
						{
							Targets.Remove(PureTarget);
							AffectedComponents.Remove(PureOtherComp);
							RemoveEffect(PureTarget, PureOtherComp);
						}
					}
				}
			}
		}
	}
}

void ULimeVesselEffect::Aim()
{
	DisableHitCone();
	AbilityState = EVesselAbilityState::Aiming;
	EnableHitCone();
}

void ULimeVesselEffect::Fire()
{
	DisableHitCone();
	AbilityState = EVesselAbilityState::Firing;
	EnableHitCone();
}

void ULimeVesselEffect::ResetAim()
{
	if (AbilityState == EVesselAbilityState::Aiming)
	{
		AbilityState = EVesselAbilityState::Idle;
		DisableHitCone();
	}
}

void ULimeVesselEffect::ResetFire()
{
	if (AbilityState == EVesselAbilityState::Firing)
	{
		AbilityState = EVesselAbilityState::Idle;
		DisableHitCone();
	}
}

void ULimeVesselEffect::EnableHitCone()
{
	HitCone->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	HitCone->SetVisibility(true);
}

void ULimeVesselEffect::DisableHitCone()
{
	HitCone->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitCone->SetVisibility(false);
}

void ULimeVesselEffect::ApplyEffect(UActiveVesselTarget* Target, UStaticMeshComponent* TargetComponent)
{
	if (Target && TargetComponent)
	{
		ULimeVesselTarget* PureTarget = Cast<ULimeVesselTarget>(Target);
		if (PureTarget)
		{
			ULimeTargetData* Component = Cast<ULimeTargetData>(PureTarget->GetDataOfComponent(TargetComponent));
			if (Component)
			{
				if (AbilityState == EVesselAbilityState::Firing)
					Component->ChargingState = EChargingState::Charging;
				else if (AbilityState == EVesselAbilityState::Aiming)
					Component->ChargingState = EChargingState::Depleteing;

				PureTarget->SetDataOfComponent(Component);
			}
		}
	}
}

void ULimeVesselEffect::RemoveEffect(UActiveVesselTarget* Target, UStaticMeshComponent* TargetComponent)
{
	if (Target && TargetComponent)
	{
		ULimeVesselTarget* PureTarget = Cast<ULimeVesselTarget>(Target);
		if (PureTarget)
		{
			ULimeTargetData* Component = Cast<ULimeTargetData>(PureTarget->GetDataOfComponent(TargetComponent));
			if (Component)
			{
				Component->ChargingState = EChargingState::Idle;
				PureTarget->SetDataOfComponent(Component);
			}
		}
	}
}