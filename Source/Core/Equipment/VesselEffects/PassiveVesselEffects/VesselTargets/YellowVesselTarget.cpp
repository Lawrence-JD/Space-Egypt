// Fill out your copyright notice in the Description page of Project Settings.


#include "YellowVesselTarget.h"
#include "../../../../HelperFiles/DefinedDebugHelpers.h"
#include "../../ActiveVesselEffects/Targets/LimeVesselTarget.h"
#include "../../ActiveVesselEffects/Targets/OrangeVesselTarget.h"
#include "../../ActiveVesselEffects/Targets/VioletVesselTarget.h"

// Sets default values for this component's properties
UYellowVesselTarget::UYellowVesselTarget()
{
	Init();
}

void UYellowVesselTarget::Init()
{
	DataEntryPrefix = "YellowData_";
	DataType = UYellowTargetData::StaticClass();
	CollisionChannel = YELLOW_CHANNEL;

	Super::Init();
}

// Called when the game starts
void UYellowVesselTarget::BeginPlay()
{
	Super::BeginPlay();
	for (auto& Component : PrimitiveComponents)
	{
		if (Component && Component->bIsAffectedByThisEffect)
		{
			UYellowTargetData* PureComponentData = Cast<UYellowTargetData>(Component);
			UPrimitiveComponent* PureComponent = GetComponentByName(PureComponentData->ComponentName);
			if (PureComponentData && PureComponentData)
			{
				if (PureComponentData->bIsAffectedHidden)
				{
					PureComponentData->AffectedCollisionProfileName = "YellowHidden";

					if (PureComponentData->bIsUnaffectedPhysicsSimulated)
						PureComponentData->UnaffectedCollisionProfileName = "YellowRevealedPhysics";
					else
						PureComponentData->UnaffectedCollisionProfileName = "YellowRevealedStatic";
				}
				else
				{
					PureComponentData->UnaffectedCollisionProfileName = "YellowHidden";

					if (PureComponentData->bIsAffectedPhysicsSimulated)
						PureComponentData->AffectedCollisionProfileName = "YellowRevealedPhysics";
					else
						PureComponentData->AffectedCollisionProfileName = "YellowRevealedStatic";
				}

				// Other effect response generation
				if (Owner)
				{
					UVesselTarget* PureTarget;
					UTargetData* PureTargetData;
					TArray<UVesselTarget*> VesselTargetFinder;
					Owner->GetComponents<UVesselTarget>(VesselTargetFinder);
					for (auto& Target : VesselTargetFinder)
					{
						PureTarget = Cast<UCyanVesselTarget>(Target);
						if (PureTarget && PureTarget->Owner)
						{
							PureTargetData = Cast<UCyanTargetData>(PureTarget->GetDataOfComponent(PureComponent));
							if(PureTargetData && PureTargetData->bIsAffectedByThisEffect)
								PureComponentData->CyanResponse = ECR_Overlap;
							continue;
						}
						PureTarget = Cast<UMagentaVesselTarget>(Target);
						if (PureTarget && PureTarget->Owner)
						{
							PureTargetData = Cast<UMagentaTargetData>(PureTarget->GetDataOfComponent(PureComponent));
							if (PureTargetData && PureTargetData->bIsAffectedByThisEffect)
								PureComponentData->MagentaResponse = ECR_Overlap;
							continue;
						}
						PureTarget = Cast<ULimeVesselTarget>(Target);
						if (PureTarget && PureTarget->Owner)
						{
							PureTargetData = Cast<ULimeTargetData>(PureTarget->GetDataOfComponent(PureComponent));
							if (PureTargetData && PureTargetData->bIsAffectedByThisEffect)
								PureComponentData->LimeResponse = ECR_Overlap;
							continue;
						}
						PureTarget = Cast<UOrangeVesselTarget>(Target);
						if (PureTarget && PureTarget->Owner)
						{
							PureTargetData = Cast<UOrangeTargetData>(PureTarget->GetDataOfComponent(PureComponent));
							if (PureTargetData && PureTargetData->bIsAffectedByThisEffect)
								PureComponentData->OrangeResponse = ECR_Block;
							continue;
						}
						PureTarget = Cast<UVioletVesselTarget>(Target);
						if (PureTarget && PureTarget->Owner)
						{
							PureTargetData = Cast<UVioletTargetData>(PureTarget->GetDataOfComponent(PureComponent));
							if (PureTargetData && PureTargetData->bIsAffectedByThisEffect)
								PureComponentData->VioletResponse = ECR_Block;
							continue;
						}
					}
				}
				PureComponentData->InteractResponse = PureComponent->GetCollisionResponseToChannel(INTERACT_CHANNEL);

				// Cyan response settings
				if (PureComponentData->CyanResponse == ECR_Overlap)
				{
					if (PureComponentData->bIsAffectedHidden)
						PureComponentData->bIsUnaffectedPhysicsSimulated = true;
					else
						PureComponentData->bIsAffectedPhysicsSimulated = true;
				}


				PureComponent->SetSimulatePhysics(PureComponentData->bIsUnaffectedPhysicsSimulated);
				PureComponent->SetCollisionProfileName(PureComponentData->UnaffectedCollisionProfileName);

				if (PureComponentData->bIsAffectedHidden)
				{
					PureComponent->SetCollisionResponseToChannel(CYAN_CHANNEL, PureComponentData->CyanResponse);
					PureComponent->SetCollisionResponseToChannel(MAGENTA_CHANNEL, PureComponentData->MagentaResponse);
					PureComponent->SetCollisionResponseToChannel(LIME_CHANNEL, PureComponentData->LimeResponse);
					PureComponent->SetCollisionResponseToChannel(ORANGE_CHANNEL, PureComponentData->OrangeResponse);
					PureComponent->SetCollisionResponseToChannel(VIOLET_CHANNEL, PureComponentData->VioletResponse);
					PureComponent->SetCollisionResponseToChannel(INTERACT_CHANNEL, PureComponentData->InteractResponse);
				}
				else
				{
					PureComponent->SetCollisionResponseToChannel(CYAN_CHANNEL, ECR_Ignore);
					PureComponent->SetCollisionResponseToChannel(MAGENTA_CHANNEL, ECR_Ignore);
					PureComponent->SetCollisionResponseToChannel(LIME_CHANNEL, ECR_Ignore);
					PureComponent->SetCollisionResponseToChannel(ORANGE_CHANNEL, ECR_Ignore);
					PureComponent->SetCollisionResponseToChannel(VIOLET_CHANNEL, ECR_Ignore);
					PureComponent->SetCollisionResponseToChannel(INTERACT_CHANNEL, ECR_Ignore);
				}


				PureComponent->SetVisibility(PureComponentData->bIsAffectedHidden, false);
			}
		}
	}
}