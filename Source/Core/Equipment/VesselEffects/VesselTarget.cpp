// Fill out your copyright notice in the Description page of Project Settings.


#include "VesselTarget.h"
#include "PassiveVesselEffects/VesselTargets/PassiveVesselTarget.h"
#include "PassiveVesselEffects/VesselTargets/CyanVesselTarget.h"
#include "PassiveVesselEffects/VesselTargets/MagentaVesselTarget.h"
#include "PassiveVesselEffects/VesselTargets/YellowVesselTarget.h"
#include "ActiveVesselEffects/Targets/ActiveVesselTarget.h"
#include "ActiveVesselEffects/Targets/LimeVesselTarget.h"
#include "ActiveVesselEffects/Targets/OrangeVesselTarget.h"
#include "ActiveVesselEffects/Targets/VioletVesselTarget.h"
#include "ActiveVesselEffects/Targets/VioletTeleportTarget.h"


// Sets default values for this component's properties
UVesselTarget::UVesselTarget()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

#if WITH_EDITOR
	bTickInEditor = true;
#endif
}

void UVesselTarget::Init()
{
	if (GetWorld() && DataType)
	{
		UTargetData* PrimitiveComponentData = NewObject<UTargetData>();

		Owner = GetOwner();
		if (Owner && PrimitiveComponents.Num() <= 0)
		{
			TArray<UStaticMeshComponent*> PrimitiveComponentFinder;
			Owner->GetComponents<UStaticMeshComponent>(PrimitiveComponentFinder);

			for (auto& Component : PrimitiveComponentFinder)
			{
				if (Component)
				{
					FString NewEntryName = DataEntryPrefix;
					NewEntryName.Append(Component->GetName());
					PrimitiveComponentData = NewObject<UTargetData>(this, DataType, FName(*NewEntryName));
					Component->SetGenerateOverlapEvents(true);

					PrimitiveComponentData->RegisterComponent();
					PrimitiveComponentData->bIsAffectedByThisEffect = true;
					PrimitiveComponentData->ComponentName = Component->GetName();

					PrimitiveComponents.Add(PrimitiveComponentData);
				}
			}
		}
	}
}

void UVesselTarget::UnInit()
{
	PrimitiveComponents.Empty();
	if(Owner)
		Owner = nullptr;
}

// Called when the game starts
void UVesselTarget::BeginPlay()
{
	Super::BeginPlay();

	if (Owner)
	{
		for (auto& Component : PrimitiveComponents)
		{
			if (Component)
			{
				UPrimitiveComponent* Primitive = GetComponentByName(Component->ComponentName);
				if (Primitive)
				{
					if (Component->bIsAffectedByThisEffect)
					{
						if (Cast<UPassiveVesselTarget>(this) || Cast<ULimeVesselTarget>(this) || Cast<UVioletTeleportTarget>(this))
							Primitive->SetCollisionResponseToChannel(CollisionChannel, ECR_Overlap);
						if (Cast<UOrangeVesselTarget>(this) || Cast<UVioletVesselTarget>(this))
							Primitive->SetCollisionResponseToChannel(CollisionChannel, ECR_Block);
					}
					else
					{
						Primitive->SetCollisionResponseToChannel(CollisionChannel, ECR_Ignore);
					}
				}
			}
		}
	}
	else
		//SetComponentTickEnabled(false);
		DestroyComponent();
}

// Called every frame
void UVesselTarget::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

#if WITH_EDITOR
	RefreshComponentData();
#endif
}

void UVesselTarget::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	for (auto& Component : PrimitiveComponents)
	{
		if (Component)
		{
			UPrimitiveComponent* Primitive = GetComponentByName(Component->GetName());
			if (Primitive)
				Primitive->SetCollisionResponseToChannel(CollisionChannel, ECR_Ignore);
		}
	}
}

#if WITH_EDITOR
void UVesselTarget::RefreshComponentData()
{
	if (Owner)
	{
		TArray<UPrimitiveComponent*> PrimitiveComponentFinder;
		Owner->GetComponents<UPrimitiveComponent>(PrimitiveComponentFinder);

		int MyNum = PrimitiveComponents.Num();

		if (MyNum == 0)
		{
			Init();
			return;
		}

		int TheirNum = PrimitiveComponentFinder.Num();

		// looking for components to add
		for (int m = 0; m < TheirNum; m++)
		{
			for (int n = 0; n < MyNum; n++)
			{
				UPrimitiveComponent* Primitive = GetComponentByName(PrimitiveComponentFinder[m]->GetName());
				if (Primitive && !GetDataOfComponent(Primitive))
				{
					FString NewEntryName = DataEntryPrefix;
					NewEntryName.Append(Primitive->GetName());

					UTargetData* PrimitiveComponentData = NewObject<UTargetData>(this, DataType, *NewEntryName);
					PrimitiveComponentData->RegisterComponent();

					PrimitiveComponentData->ComponentName = Primitive->GetName();
					PrimitiveComponents.Add(PrimitiveComponentData);
				}
			}
		}

		// looking for components to remove 
		MyNum = PrimitiveComponents.Num();
		TArray<UTargetData*> NewComponents = PrimitiveComponents;
		for (auto& Component : PrimitiveComponents)
		{
			if (Component)
			{
				UPrimitiveComponent* Primitive = GetComponentByName(Component->ComponentName);
				if (!Primitive || Primitive->IsPendingKillOrUnreachable())
				{
					NewComponents.Remove(Component);
				}
			}
			else
				NewComponents.Remove(Component);
		}
		PrimitiveComponents = NewComponents;
	}
}
#endif

UTargetData* UVesselTarget::GetDataOfComponent(UPrimitiveComponent* Component)
{
	for (auto& Entry : PrimitiveComponents)
	{
		if (Owner && Component && Entry && Component->GetOwner() && Owner->GetName() == Component->GetOwner()->GetName() && Entry->ComponentName == Component->GetName())
		{
			return Entry;
		}
	}
	return nullptr;
}

UPrimitiveComponent* UVesselTarget::GetComponentByName(FString Name)
{

	TArray<UStaticMeshComponent*> PrimitiveComponentFinder;
	Owner->GetComponents<UStaticMeshComponent>(PrimitiveComponentFinder);
	for (auto& Component : PrimitiveComponentFinder)
	{
		if (Component && Component->GetName() == Name)
			return Component;
	}
	return nullptr;
}

bool UVesselTarget::SetDataOfComponent(UTargetData* Component)
{
	for (auto& Entry : PrimitiveComponents)
	{
		if (Entry && Component && Entry->ComponentName == Component->ComponentName)
		{
			Entry = Component;
			return true;
		}
	}
	return false;
}