// Fill out your copyright notice in the Description page of Project Settings.


#include "YellowGroupTarget.h"
#include "../../TargetData/YellowTargetData.h"
#include "../CyanVesselTarget.h"
#include "../MagentaVesselTarget.h"
#include "../../../ActiveVesselEffects/Targets/LimeVesselTarget.h"
#include "../../../ActiveVesselEffects/Targets/OrangeVesselTarget.h"
#include "../../../ActiveVesselEffects/Targets/VioletVesselTarget.h"

// Sets default values for this component's properties
UYellowGroupTarget::UYellowGroupTarget()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	Init();
}

void UYellowGroupTarget::Init()
{
	DataEntryPrefix = "YellowGroupData_";
	DataType = UYellowTargetData::StaticClass();
	CollisionChannel = YELLOW_CHANNEL;

	Initialize();
}

// Called when the game starts
// Called when the game starts
void UYellowGroupTarget::BeginPlay()
{
	Super::BeginPlay();

	for (auto& Component : PrimitiveComponents)
	{
		if (Component && Component->bIsAffectedByThisEffect)
		{
			UYellowTargetData* PureComponentData = Cast<UYellowTargetData>(Component);
			if (PureComponentData)
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
					for (auto Name : PureComponentData->GroupMembers)
					{
						UPrimitiveComponent* NewComponent = GetComponentByName(Name);
						if (NewComponent)
						{
							UStaticMeshComponent* PureComponent = Cast<UStaticMeshComponent>(NewComponent);
							if (PureComponent)
							{
								for (auto& Target : VesselTargetFinder)

								{
									//	PureTarget = Cast<UCyanVesselTarget>(Target);
									//	if (PureTarget && PureTarget->Owner)
									//	{
									//		PureTargetData = Cast<UCyanTargetData>(PureTarget->GetDataOfComponent(PureComponent));
									//		if (PureTargetData && PureTargetData->bIsAffectedByThisEffect)
									//			PureComponentData->CyanResponse = ECR_Overlap;
									//		continue;
									//	}
									//	PureTarget = Cast<UMagentaVesselTarget>(Target);
									//	if (PureTarget && PureTarget->Owner)
									//	{
									//		PureTargetData = Cast<UMagentaTargetData>(PureTarget->GetDataOfComponent(PureComponent));
									//		if (PureTargetData && PureTargetData->bIsAffectedByThisEffect)
									//			PureComponentData->MagentaResponse = ECR_Overlap;
									//		continue;
									//	}
									//	PureTarget = Cast<ULimeVesselTarget>(Target);
									//	if (PureTarget && PureTarget->Owner)
									//	{
									//		PureTargetData = Cast<ULimeTargetData>(PureTarget->GetDataOfComponent(PureComponent));
									//		if (PureTargetData && PureTargetData->bIsAffectedByThisEffect)
									//			PureComponentData->LimeResponse = ECR_Overlap;
									//		continue;
									//	}
									PureTarget = Cast<UOrangeVesselTarget>(Target);
									if (PureTarget && PureTarget->Owner)
									{
										PureTargetData = Cast<UOrangeTargetData>(PureTarget->GetDataOfComponent(PureComponent));
										if (PureTargetData && PureTargetData->bIsAffectedByThisEffect)
											PureComponentData->OrangeResponse = ECR_Block;
										//continue;
									}
									//	PureTarget = Cast<UVioletVesselTarget>(Target);
									//	if (PureTarget && PureTarget->Owner)
									//	{
									//		PureTargetData = Cast<UVioletTargetData>(PureTarget->GetDataOfComponent(PureComponent));
									//		if (PureTargetData && PureTargetData->bIsAffectedByThisEffect)
									//			PureComponentData->VioletResponse = ECR_Block;
									//		continue;
									//	}
								}
								if (PureComponentData->bIsAffectedHidden)
								{
									//PureComponent->SetCollisionResponseToChannel(CYAN_CHANNEL, PureComponentData->CyanResponse);
									//PureComponent->SetCollisionResponseToChannel(MAGENTA_CHANNEL, PureComponentData->MagentaResponse);
									//PureComponent->SetCollisionResponseToChannel(LIME_CHANNEL, PureComponentData->LimeResponse);
									PureComponent->SetCollisionResponseToChannel(ORANGE_CHANNEL, PureComponentData->OrangeResponse);
									//PureComponent->SetCollisionResponseToChannel(VIOLET_CHANNEL, PureComponentData->VioletResponse);
									//PureComponent->SetCollisionResponseToChannel(INTERACT_CHANNEL, PureComponentData->InteractResponse);
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
								PureComponent->SetCollisionProfileName(PureComponentData->UnaffectedCollisionProfileName);

							}
						}
					}
				}
			}
			//PureComponentData->InteractResponse = PureComponent->GetCollisionResponseToChannel(INTERACT_CHANNEL);

			// Cyan response settings
			//if (PureComponentData->CyanResponse == ECR_Overlap)
			//{
				//if (PureComponentData->bIsAffectedHidden)
					//PureComponentData->bIsUnaffectedPhysicsSimulated = true;
				//else
					//PureComponentData->bIsAffectedPhysicsSimulated = true;
			//}


			//PureComponent->SetSimulatePhysics(PureComponentData->bIsUnaffectedPhysicsSimulated);
			//PureComponent->SetCollisionProfileName(PureComponentData->UnaffectedCollisionProfileName);
		}
	}
}

// Called every frame
void UYellowGroupTarget::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

