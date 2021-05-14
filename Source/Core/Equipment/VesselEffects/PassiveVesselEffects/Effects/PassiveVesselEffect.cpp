// Fill out your copyright notice in the Description page of Project Settings.


#include "PassiveVesselEffect.h"
#include "../../../../HelperFiles/DefinedDebugHelpers.h"

// Sets default values for this component's properties
UPassiveVesselEffect::UPassiveVesselEffect()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PostPhysics;

	EffectShape = CreateDefaultSubobject<UEffectShape>(TEXT("EffectShape"));
	BoxEffectShape = CreateDefaultSubobject<UBoxEffectShape>(TEXT("BoxEffectShape"));
	CapsuleEffectShape = CreateDefaultSubobject<UCapsuleEffectShape>(TEXT("CapsuleEffectShape"));
	ConeEffectShape = CreateDefaultSubobject<UConeEffectShape>(TEXT("ConeEffectShape"));
	OrbEffectShape = CreateDefaultSubobject<USphereEffectShape>(TEXT("OrbEffectShape"));
	
	EffectShape->SetupAttachment(this);
	BoxEffectShape->SetupAttachment(this);
	CapsuleEffectShape->SetupAttachment(this);
	ConeEffectShape->SetupAttachment(this);
	OrbEffectShape->SetupAttachment(this);
}

void UPassiveVesselEffect::PostLoad()
{
	Super::PostLoad();
}

// Called when the game starts
void UPassiveVesselEffect::BeginPlay()
{
	Super::BeginPlay();

	EffectShape->EffectShapeMesh->OnComponentEndOverlap.AddDynamic(this, &UPassiveVesselEffect::OnOverlapEnd);
}

// Called every frame, affects unaffected yellow objects
void UPassiveVesselEffect::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TArray<UPrimitiveComponent*> NewTargets;
	if(EffectShape)
		EffectShape->EffectShapeMesh->GetOverlappingComponents(NewTargets);
	for (auto& Target : NewTargets)
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
					UPassiveVesselTarget* PureTarget = Cast<UPassiveVesselTarget>(VesselTarget);
					UGroupVesselTarget* OtherPureTarget = Cast<UGroupVesselTarget>(VesselTarget);
					if (PureTarget)
					{
						if (!Targets.Contains(PureTarget))
							Targets.Add(PureTarget);
						if (!AffectedComponents.Contains(NewTarget))
							AffectedComponents.Add(NewTarget);
						//SCREENMSG("ADD");
						//SCREENMSG(NewTarget->GetName()); 
						if (OtherPureTarget)
							ApplyEffectToGroup(OtherPureTarget, OtherPureTarget->GetDataOfComponent(NewTarget));
						else
							ApplyEffect(PureTarget, NewTarget);
					}
				}
			}
		}
	}
}

// Removes affected objects
void UPassiveVesselEffect::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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
					UPassiveVesselTarget* PureTarget = Cast<UPassiveVesselTarget>(Target);
					UGroupVesselTarget* OtherPureTarget = Cast<UGroupVesselTarget>(Target);
					if (PureTarget)
					{
						Targets.Remove(PureTarget);
						AffectedComponents.Remove(PureOtherComp);

						//SCREENMSG("REMOVE");
						//SCREENMSG(TargetData->Actor->GetName());
						if(OtherPureTarget)
							RemoveEffectFromGroup(OtherPureTarget, OtherPureTarget->GetDataOfComponent(PureOtherComp));
						else
							RemoveEffect(PureTarget, PureOtherComp);
					}
				}
			}
		}
	}
}

// Used when switching vessel effects
void UPassiveVesselEffect::Enable()
{
	Super::Enable();
	EffectShape->Enable();
}

// Used when switching vessel effects
void UPassiveVesselEffect::Disable()
{
	Super::Disable();
	EffectShape->Disable();
}

// Base function, should be overridden
void UPassiveVesselEffect::ApplyEffect(UPassiveVesselTarget* Target, UStaticMeshComponent* TargetComponent)
{
	;
}

// Base function, should be overridden
void UPassiveVesselEffect::RemoveEffect(UPassiveVesselTarget* Target, UStaticMeshComponent* TargetComponent)
{
	;
}

void UPassiveVesselEffect::ApplyEffectToGroup(UPassiveVesselTarget* Target, UTargetData* Data)
{
	;
}

void UPassiveVesselEffect::RemoveEffectFromGroup(UPassiveVesselTarget* Target, UTargetData* Data)
{
	;
}

void UPassiveVesselEffect::SetEffectShapeType(EEffectShapeType NewType)
{
	if (BoxEffectShape && CapsuleEffectShape && ConeEffectShape && OrbEffectShape)
	{
		EffectShapeType = NewType;
		EffectShape->Decal->DestroyComponent();
		EffectShape->EffectShapeMesh->DestroyComponent();
		EffectShape->DestroyComponent();

		switch (EffectShapeType)
		{
		case EEffectShapeType::Box:
			EffectShape = BoxEffectShape;
			CapsuleEffectShape->Terminate();
			ConeEffectShape->Terminate();
			OrbEffectShape->Terminate();
			break;
		case EEffectShapeType::Capsule:
			EffectShape = CapsuleEffectShape;
			BoxEffectShape->Terminate();
			ConeEffectShape->Terminate();
			OrbEffectShape->Terminate();
			break;
		case EEffectShapeType::Cone:
			EffectShape = ConeEffectShape;
			CapsuleEffectShape->Terminate();
			BoxEffectShape->Terminate();
			OrbEffectShape->Terminate();
			break;
		case EEffectShapeType::Sphere:
			EffectShape = OrbEffectShape;
			CapsuleEffectShape->Terminate();
			BoxEffectShape->Terminate();
			ConeEffectShape->Terminate();
			break;
		}
		EffectShape->EffectShapeMesh->SetCollisionProfileName(FName(CollisionProfileName));
		EffectShape->GlowMat = GlowMat;
		EffectShape->DecalColor = DecalColor;
	}
}
