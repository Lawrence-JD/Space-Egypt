// Fill out your copyright notice in the Description page of Project Settings.


#include "MagentaVesselEffect.h"
#include "../../../../HelperFiles/DefinedDebugHelpers.h"

// Sets default values for this component's properties
UMagentaVesselEffect::UMagentaVesselEffect()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	CollisionProfileName = "MagentaVessel";

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> GlowMatFinder(TEXT("MaterialInstanceConstant'/Game/Speegypt/Effects/Materials/MAT_MagentaGlow_Inst.MAT_MagentaGlow_Inst'"));
	if (GlowMatFinder.Object)
	{
		GlowMat = GlowMatFinder.Object;
		DecalColor = MAGENTA_BASE_COLOR;
	}
}


// Called when the game starts
void UMagentaVesselEffect::BeginPlay()
{
	Super::BeginPlay();

}


// Called every frame
void UMagentaVesselEffect::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//SCREENMSG("MAGENTA");
}

void UMagentaVesselEffect::ApplyEffect(UPassiveVesselTarget* Target, UStaticMeshComponent* TargetComponent)
{
	UMagentaVesselTarget* PureTarget = Cast<UMagentaVesselTarget>(Target);
	if (PureTarget)
	{
		UMagentaTargetData* Component = Cast<UMagentaTargetData>(PureTarget->GetDataOfComponent(TargetComponent));
		if (Component)
		{
			PureTarget->AddSource(TargetComponent);
			Component->bIsFixed = true;
			PureTarget->SetDataOfComponent(Component);
		}
	}
}

void UMagentaVesselEffect::RemoveEffect(UPassiveVesselTarget* Target, UStaticMeshComponent* TargetComponent)
{
	UMagentaVesselTarget* PureTarget = Cast<UMagentaVesselTarget>(Target);
	if (PureTarget && PureTarget->RemoveSource(TargetComponent) == 2)
	{
		UMagentaTargetData* Component = Cast<UMagentaTargetData>(PureTarget->GetDataOfComponent(TargetComponent));
		if (Component)
		{
			Component->bIsFixed = false;
			PureTarget->SetDataOfComponent(Component);
		}
	}
}
