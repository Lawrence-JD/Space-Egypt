// Fill out your copyright notice in the Description page of Project Settings.


#include "YellowVesselEffect.h"
#include "../../ActiveVesselEffects/Misc/RockPillar.h"
#include "../../../../HelperFiles/DefinedDebugHelpers.h"


// Sets default values for this component's properties
UYellowVesselEffect::UYellowVesselEffect()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	CollisionProfileName = "YellowVessel";

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> GlowMatFinder(TEXT("MaterialInstanceConstant'/Game/Speegypt/Effects/Materials/MAT_YellowGlow_Inst.MAT_YellowGlow_Inst'"));
	if (GlowMatFinder.Object)
	{
		GlowMat = GlowMatFinder.Object;
		DecalColor = YELLOW_BASE_COLOR;
	}
}

// Called when the game starts
void UYellowVesselEffect::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void UYellowVesselEffect::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//SCREENMSG("YELLOW");
}

void UYellowVesselEffect::ApplyEffect(UPassiveVesselTarget* Target, UStaticMeshComponent* TargetComponent)
{
	//SCREENMSG("APPLY YELLOW EFFECT");
	if (Target && TargetComponent)
	{
		UYellowVesselTarget* PureTarget = Cast<UYellowVesselTarget>(Target);
		if (PureTarget)
		{
			PureTarget->AddSource(TargetComponent);
			UYellowTargetData* PureComponent = Cast<UYellowTargetData>(PureTarget->GetDataOfComponent(TargetComponent));
			if (PureComponent)
			{
				TargetComponent->SetCollisionProfileName(PureComponent->AffectedCollisionProfileName);
				TargetComponent->SetSimulatePhysics(PureComponent->bIsAffectedPhysicsSimulated);
				TargetComponent->SetVisibility(!PureComponent->bIsAffectedHidden, true);

				if (!PureComponent->bIsAffectedHidden)
				{
					TargetComponent->SetCollisionResponseToChannel(CYAN_CHANNEL, PureComponent->CyanResponse);
					TargetComponent->SetCollisionResponseToChannel(MAGENTA_CHANNEL, PureComponent->MagentaResponse);
					TargetComponent->SetCollisionResponseToChannel(LIME_CHANNEL, PureComponent->LimeResponse);
					TargetComponent->SetCollisionResponseToChannel(ORANGE_CHANNEL, PureComponent->OrangeResponse);
					TargetComponent->SetCollisionResponseToChannel(VIOLET_CHANNEL, PureComponent->VioletResponse);
					TargetComponent->SetCollisionResponseToChannel(INTERACT_CHANNEL, PureComponent->InteractResponse);
				}
			}
		}
	}
}

void UYellowVesselEffect::RemoveEffect(UPassiveVesselTarget* Target, UStaticMeshComponent* TargetComponent)
{
	//SCREENMSG("REMOVE YELLOW EFFECT");
	if (Target && TargetComponent)
	{
		UYellowVesselTarget* PureTarget = Cast<UYellowVesselTarget>(Target);
		if (PureTarget && PureTarget->RemoveSource(TargetComponent) == 2)
		{
			UYellowTargetData* Component = Cast<UYellowTargetData>(PureTarget->GetDataOfComponent(TargetComponent));
			if (Component)
			{
				TargetComponent->SetCollisionProfileName(Component->UnaffectedCollisionProfileName);
				TargetComponent->SetSimulatePhysics(Component->bIsUnaffectedPhysicsSimulated);
				TargetComponent->SetVisibility(Component->bIsAffectedHidden, true);

				if (Component->bIsAffectedHidden)
				{
					TargetComponent->SetCollisionResponseToChannel(CYAN_CHANNEL, Component->CyanResponse);
					TargetComponent->SetCollisionResponseToChannel(MAGENTA_CHANNEL, Component->MagentaResponse);
					TargetComponent->SetCollisionResponseToChannel(LIME_CHANNEL, Component->LimeResponse);
					TargetComponent->SetCollisionResponseToChannel(ORANGE_CHANNEL, Component->OrangeResponse);
					TargetComponent->SetCollisionResponseToChannel(VIOLET_CHANNEL, Component->VioletResponse);
					TargetComponent->SetCollisionResponseToChannel(INTERACT_CHANNEL, Component->InteractResponse);
				}
			}
		}
	}
}

void UYellowVesselEffect::ApplyEffectToGroup(UPassiveVesselTarget* Target, UTargetData* Data)
{
	//SCREENMSG("APPLY YELLOW EFFECT");

	if (Target && Data)
	{
		UYellowGroupTarget* PureTarget = Cast<UYellowGroupTarget>(Target);
		UYellowTargetData* PureData = Cast<UYellowTargetData>(Data);
		if (PureTarget && PureData)
		{
			PureTarget->AddSource(PureData);

			for (auto& Name : PureData->GroupMembers)
			{
				UPrimitiveComponent* TargetComponent = PureTarget->GetComponentByName(Name);
				if (TargetComponent)
				{
					TargetComponent->SetCollisionProfileName(PureData->AffectedCollisionProfileName);
					TargetComponent->SetSimulatePhysics(PureData->bIsAffectedPhysicsSimulated);
					TargetComponent->SetVisibility(!PureData->bIsAffectedHidden, true);

					if (!PureData->bIsAffectedHidden)
					{
						TargetComponent->SetCollisionResponseToChannel(CYAN_CHANNEL, PureData->CyanResponse);
						TargetComponent->SetCollisionResponseToChannel(MAGENTA_CHANNEL, PureData->MagentaResponse);
						TargetComponent->SetCollisionResponseToChannel(LIME_CHANNEL, PureData->LimeResponse);
						TargetComponent->SetCollisionResponseToChannel(ORANGE_CHANNEL, PureData->OrangeResponse);
						TargetComponent->SetCollisionResponseToChannel(VIOLET_CHANNEL, PureData->VioletResponse);
						TargetComponent->SetCollisionResponseToChannel(INTERACT_CHANNEL, PureData->InteractResponse);
					}
				}
			}
		}
	}
}

void UYellowVesselEffect::RemoveEffectFromGroup(UPassiveVesselTarget* Target, UTargetData* Data)
{
	//SCREENMSG("REMOVE YELLOW EFFECT");

	if (Target && Data)
	{
		UYellowGroupTarget* PureTarget = Cast<UYellowGroupTarget>(Target);
		UYellowTargetData* PureData = Cast<UYellowTargetData>(Data);
		if (PureTarget && PureData && PureTarget->RemoveSource(PureData) == 2)
		{
			for (auto& Name : PureData->GroupMembers)
			{
				UPrimitiveComponent* TargetComponent = PureTarget->GetComponentByName(Name);
				TargetComponent->SetCollisionProfileName(PureData->UnaffectedCollisionProfileName);
				TargetComponent->SetSimulatePhysics(PureData->bIsUnaffectedPhysicsSimulated);
				TargetComponent->SetVisibility(PureData->bIsAffectedHidden, true);

				if (PureData->bIsAffectedHidden)
				{
					TargetComponent->SetCollisionResponseToChannel(CYAN_CHANNEL, PureData->CyanResponse);
					TargetComponent->SetCollisionResponseToChannel(MAGENTA_CHANNEL, PureData->MagentaResponse);
					TargetComponent->SetCollisionResponseToChannel(LIME_CHANNEL, PureData->LimeResponse);
					TargetComponent->SetCollisionResponseToChannel(ORANGE_CHANNEL, PureData->OrangeResponse);
					TargetComponent->SetCollisionResponseToChannel(VIOLET_CHANNEL, PureData->VioletResponse);
					TargetComponent->SetCollisionResponseToChannel(INTERACT_CHANNEL, PureData->InteractResponse);
				}
			}
		}
	}
}
