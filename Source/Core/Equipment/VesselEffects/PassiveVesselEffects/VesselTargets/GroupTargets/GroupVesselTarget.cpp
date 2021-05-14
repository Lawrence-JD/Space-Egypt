// Fill out your copyright notice in the Description page of Project Settings.


#include "GroupVesselTarget.h"

// Sets default values for this component's properties
UGroupVesselTarget::UGroupVesselTarget()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UGroupVesselTarget::Initialize()
{
	if (GetWorld() && DataType)
	{
		UPassiveTargetData* SceneComponentData = NewObject<UPassiveTargetData>();

		Owner = GetOwner();
		if (Owner && PrimitiveComponents.Num() <= 0)
		{
			TArray<USceneComponent*> SceneComponentFinder;
			TArray<USceneComponent*> GroupMemberFinder;
			Owner->GetComponents<USceneComponent>(SceneComponentFinder);

			for (auto& Component : SceneComponentFinder)
			{
				if (Component && Component->GetName().Contains("Group"))
				{
					FString NewEntryName = DataEntryPrefix;
					NewEntryName.Append(Component->GetName());
					SceneComponentData = NewObject<UPassiveTargetData>(this, DataType, FName(*NewEntryName));

					SceneComponentData->RegisterComponent();
					SceneComponentData->bIsAffectedByThisEffect = true;
					SceneComponentData->ComponentName = Component->GetName();

					GroupMemberFinder = Component->GetAttachChildren();

					for (auto& Member : GroupMemberFinder)
					{
						if (Member)
						{
							UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(Member);
							if (Mesh)
							{
								SceneComponentData->GroupMembers.Add(Mesh->GetName());
								Mesh->SetGenerateOverlapEvents(true);
							}
						}
					}
					PrimitiveComponents.Add(SceneComponentData);
				}
			}
		}
	}
}

// Called when the game starts
void UGroupVesselTarget::BeginPlay()
{
	Super::BeginPlay();

	if (Owner)
	{
		for (auto& Component : PrimitiveComponents)
		{
			if (Component)
			{
				UPassiveTargetData* Data = Cast<UPassiveTargetData>(Component);
				if (Data)
				{
					for (auto& Name : Data->GroupMembers)
					{
						UPrimitiveComponent* Primitive = GetComponentByName(Name);
						if (Primitive)
						{
							if (Component->bIsAffectedByThisEffect)
								Primitive->SetCollisionResponseToChannel(CollisionChannel, ECR_Overlap);
							else
								Primitive->SetCollisionResponseToChannel(CollisionChannel, ECR_Ignore);
						}
					}
				}
			}
		}
	}
}


// Called every frame
void UGroupVesselTarget::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

UTargetData* UGroupVesselTarget::GetDataOfComponent(UPrimitiveComponent* Component)
{
	if (Owner && Component && Component->GetOwner() && Owner == Component->GetOwner())
	{
		for (auto& Entry : PrimitiveComponents)
		{
			if (Entry)
			{
				UPassiveTargetData* PureEntry = Cast<UPassiveTargetData>(Entry);
				if (PureEntry && PureEntry->GroupMembers.Contains(Component->GetName()))
				{
					return Entry;
				}
			}
		}
	}
	return nullptr;
}

UTargetData* UGroupVesselTarget::GetDataOfSceneComponent(USceneComponent* Component)
{
	if (Owner && Component && Component->GetOwner() && Owner == Component->GetOwner())
	{
		for (auto& Entry : PrimitiveComponents)
		{
			if (Entry && Entry->ComponentName == Component->GetName())
			{
				return Entry;
			}
		}
	}
	return nullptr;
}

USceneComponent* UGroupVesselTarget::GetSceneComponentByName(FString Name)
{

	TArray<USceneComponent*> SceneComponentFinder;
	Owner->GetComponents<USceneComponent>(SceneComponentFinder);
	for (auto& Component : SceneComponentFinder)
	{
		if (Component && Component->GetName() == Name)
			return Component;
	}
	return nullptr;
}

#if WITH_EDITOR
void UGroupVesselTarget::RefreshComponentData()
{
	if (Owner)
	{
		TArray<USceneComponent*> SceneComponentFinder;
		Owner->GetComponents<USceneComponent>(SceneComponentFinder);

		int MyNum = PrimitiveComponents.Num();

		if (MyNum == 0)
		{
			Init();
			return;
		}

		int TheirNum = SceneComponentFinder.Num();

		// looking for group headers to add
		for (int m = 0; m < TheirNum; m++)
		{
			for (int n = 0; n < MyNum; n++)
			{
				USceneComponent* SceneComponent = GetSceneComponentByName(SceneComponentFinder[m]->GetName());
				if (SceneComponent && SceneComponent->GetName().Contains("Group") && !GetDataOfSceneComponent(SceneComponent))
				{
					FString NewEntryName = DataEntryPrefix;
					NewEntryName.Append(SceneComponent->GetName());

					UPassiveTargetData* PrimitiveComponentData = NewObject<UPassiveTargetData>(this, DataType, *NewEntryName);
					PrimitiveComponentData->RegisterComponent();

					PrimitiveComponentData->ComponentName = SceneComponent->GetName();
					PrimitiveComponents.Add(PrimitiveComponentData);
				}
			}
		}
		
		//looking for group members to add
		TArray<UStaticMeshComponent*> StaticMeshComponentFinder;
		Owner->GetComponents<UStaticMeshComponent>(StaticMeshComponentFinder);
		for (auto& Component : StaticMeshComponentFinder)
		{
			if (Component && !GetDataOfComponent(Component) && Component->GetAttachParent())
			{
				UTargetData* GroupData = GetDataOfSceneComponent(Component->GetAttachParent());
				if (GroupData)
				{
					UPassiveTargetData* PureGroupData = Cast<UPassiveTargetData>(GroupData);
					if (PureGroupData)
					{
						PureGroupData->GroupMembers.Add(Component->GetName());
					}
				}
			}
		}
		
		// looking for groups to remove 
		TArray<UTargetData*> NewComponents = PrimitiveComponents;
		for (auto& Component : PrimitiveComponents)
		{
			if (Component)
			{
				USceneComponent* SceneComponent = GetSceneComponentByName(Component->ComponentName);
				if (!SceneComponent || SceneComponent->IsPendingKillOrUnreachable())
				{
					NewComponents.Remove(Component);
				}
			}
			else
				NewComponents.Remove(Component);
		}
		PrimitiveComponents = NewComponents;

		// looking for members to remove
		for (auto& Component : PrimitiveComponents)
		{
			if (Component)
			{
				UPassiveTargetData* PureComponent = Cast<UPassiveTargetData>(Component);
				if (PureComponent)
				{
					for (auto i = PureComponent->GroupMembers.CreateIterator(); i; i++)
					{
						if (!GetComponentByName(*i))
						{
							i.RemoveCurrent();
						}
					}
				}
			}
		}
	}
}
#endif