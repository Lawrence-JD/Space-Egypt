// Fill out your copyright notice in the Description page of Project Settings.


#include "CyanVesselEffect.h"
#include "../VesselTargets/GroupTargets/CyanGroupTarget.h"
#include "../../../../HelperFiles/DefinedDebugHelpers.h"

// Sets default values for this component's properties
UCyanVesselEffect::UCyanVesselEffect()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	CollisionProfileName = "CyanVessel";

	ForceDirection = CreateDefaultSubobject<UCyanDirection>(TEXT("ForceDirection"));

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> GlowMatFinder(TEXT("MaterialInstanceConstant'/Game/Speegypt/Effects/Materials/MAT_CyanGlow_Inst.MAT_CyanGlow_Inst'"));
	if (GlowMatFinder.Object)
	{
		GlowMat = GlowMatFinder.Object;
		DecalColor = CYAN_BASE_COLOR;
	}
}


// Called when the game starts
void UCyanVesselEffect::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner())
	{
		ForceDirection->SourcePosition = EffectShape->GetComponentLocation();
		ForceDirection->SourceMagnitude = ForceMagnitude;
	}
}

// Called every frame
void UCyanVesselEffect::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//SCREENMSG("CYAN");
	ForceDirection->CalculateForceDirection();
}

void UCyanVesselEffect::ApplyEffect(UPassiveVesselTarget* Target, UStaticMeshComponent* TargetComponent)
{
	if (Target && TargetComponent)
	{
		UCyanVesselTarget* PureTarget = Cast<UCyanVesselTarget>(Target);
		UTargetData* Data = Target->GetDataOfComponent(TargetComponent);
		if (PureTarget && Data)
		{
			UCyanTargetData* Component = Cast<UCyanTargetData>(Data);
			if (Component)
			{
				UPrimitiveComponent* Primitive = PureTarget->GetComponentByName(Component->ComponentName);
				if (Primitive)
				{
					PureTarget->AddSource(TargetComponent);

					Primitive->SetEnableGravity(false);
					Primitive->SetPhysicsLinearVelocity(FVector(Primitive->GetPhysicsLinearVelocity().X, Primitive->GetPhysicsLinearVelocity().Y, 0));
					Component->bApplyForce = true;
					ForceDirection->CalculateForceDirection();
					Component->ForceDirections.Add(ForceDirection);
					Component->ForceMagnitude += ForceMagnitude;
					Component->UpdateComponentDirection();
				}
			}
		}
	}
}

void UCyanVesselEffect::RemoveEffect(UPassiveVesselTarget* Target, UStaticMeshComponent* TargetComponent)
{
	if (Target && TargetComponent)
	{
		UCyanVesselTarget* PureTarget = Cast<UCyanVesselTarget>(Target);
		UTargetData* Data = Target->GetDataOfComponent(TargetComponent);
		if (PureTarget && Data)
		{
			UCyanTargetData* Component = Cast<UCyanTargetData>(Data);
			if (Component)
			{
				UPrimitiveComponent* Primitive = PureTarget->GetComponentByName(Component->ComponentName);
				if (Primitive)
				{
					int SourceCheck = PureTarget->RemoveSource(TargetComponent);

					Component->ForceDirections.Remove(ForceDirection);
					Component->ForceMagnitude -= ForceMagnitude;
					Component->UpdateComponentDirection();

					if (SourceCheck == 2)
					{
						Primitive->SetEnableGravity(true);
						Component->bApplyForce = false;
					}
				}
			}
		}
	}
}

void UCyanVesselEffect::ApplyEffectToGroup(UPassiveVesselTarget* Target, UTargetData* Data)
{
	if (Target && Data)
	{
		UCyanGroupTarget* PureTarget = Cast<UCyanGroupTarget>(Target);
		UCyanTargetData* PureData = Cast<UCyanTargetData>(Data);
		if (PureTarget && PureData)
		{
			PureTarget->AddSource(PureData);
			PureData->bApplyForce = true;
			ForceDirection->CalculateForceDirection();
			PureData->ForceDirections.Add(ForceDirection);
			PureData->ForceMagnitude += ForceMagnitude;
			PureData->UpdateComponentDirection();

			for (auto& Name : PureData->GroupMembers)
			{
				UPrimitiveComponent* Primitive = PureTarget->GetComponentByName(Name);
				if (Primitive)
				{
					Primitive->SetEnableGravity(false);
					//Primitive->SetPhysicsLinearVelocity(FVector(0, 0, 0));
				}
			}
		}
	}
}

void UCyanVesselEffect::RemoveEffectFromGroup(UPassiveVesselTarget* Target, UTargetData* Data)
{
	if (Target && Data)
	{
		UCyanGroupTarget* PureTarget = Cast<UCyanGroupTarget>(Target);
		UCyanTargetData* PureData = Cast<UCyanTargetData>(Data);

		if (PureTarget && PureData)
		{
			int SourceCheck = PureTarget->RemoveSource(PureData);

			PureData->ForceDirections.Remove(ForceDirection);
			PureData->ForceMagnitude -= ForceMagnitude;
			PureData->UpdateComponentDirection();

			if (SourceCheck == 2)
			{
				PureData->bApplyForce = false;

				for (auto& Name : PureData->GroupMembers)
				{
					UPrimitiveComponent* Primitive = PureTarget->GetComponentByName(Name);
					if (Primitive)
					{
						Primitive->SetEnableGravity(true);
					}
				}
			}
		}
	}
}
