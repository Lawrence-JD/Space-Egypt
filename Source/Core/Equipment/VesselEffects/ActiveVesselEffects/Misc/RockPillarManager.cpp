// Fill out your copyright notice in the Description page of Project Settings.


#include "RockPillarManager.h"

// Sets default values for this component's properties
URockPillarManager::URockPillarManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	Reticle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Reticle"));
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("StaticMesh'/Game/Speegypt/Environment/Meshes/Pillar.Pillar'"));
	if (MeshFinder.Object)
	{
		ReticleMesh = MeshFinder.Object;
		

		static ConstructorHelpers::FObjectFinder<UMaterial> MatFinder(TEXT("Material'/Game/Speegypt/Environment/Materials/MAT_RockPillarReticle.MAT_RockPillarReticle'"));
		if (MatFinder.Object)
		{
			ReticleMat = MatFinder.Object;
		}

		Reticle->SetVisibility(false);
		Reticle->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

bool URockPillarManager::SpawnPillar(FVector Location, FRotator Rotation, float Force, UPrimitiveComponent* Component)
{
	if (GetWorld())
	{
		int ExcessPillars = Pillars.Num() - MaxPillars;
		while (ExcessPillars >= 0)
		{
			DestroyPillar(Pillars[0]);
			ExcessPillars = Pillars.Num() - MaxPillars;
		}
		ARockPillar* Temp = (ARockPillar*)GetWorld()->SpawnActor(ARockPillar::StaticClass(), &Location, &Rotation);
		if (Temp)
		{
			Temp->SpawnForce = Force;
			Pillars.Add(Temp);
			if (Component)
			{
				EAttachmentRule Rule = EAttachmentRule::KeepWorld;
				FAttachmentTransformRules Rules = FAttachmentTransformRules(Rule, true);
				Temp->AttachToComponent(Component, Rules);
				Component->GetBodyInstance()->AddForce(FVector(0, 0, 0));
			}
			return true;
		}
	}
	return false;
}

void URockPillarManager::DestroyPillar(ARockPillar* Pillar)
{
	if (Pillar)
	{
		if(Pillars.Remove(Pillar))
			Pillar->Destroy();
	}
}

void URockPillarManager::DrawReticle(FVector Location, FRotator Rotation)
{
	if (Reticle)
	{
		Reticle->SetVisibility(true);
		Reticle->SetWorldLocationAndRotation(Location, Rotation);
	}
}

void URockPillarManager::HideReticle()
{
	if (Reticle)
	{
		Reticle->SetVisibility(false);
	}
}

// Called when the game starts
void URockPillarManager::BeginPlay()
{
	Super::BeginPlay();

	if (ReticleMesh)
	{
		Reticle->SetStaticMesh(ReticleMesh);
		Reticle->SetWorldScale3D(FVector(1, 1, 2));

		if (ReticleMat)
		{
			Reticle->SetMaterial(0, ReticleMat);
		}
	}
}


// Called every frame
void URockPillarManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (int i = 0; i < Pillars.Num(); i++)
	{
		ARockPillar* Pillar = Pillars[i];
		if (Pillar && !Pillar->IsPendingKill())
		{
			Pillar->CurrentLifetime += DeltaTime;

			if (!Pillar->Mesh->GetVisibleFlag() && Pillar->DespawnFrameDelay)
				Pillar->DespawnFrameDelay--;
			else
				Pillar->DespawnFrameDelay = 3;

			if (MaxPillarLifetime && (Pillar->CurrentLifetime > MaxPillarLifetime || !Pillar->DespawnFrameDelay))
				DestroyPillar(Pillar);
		}
		else
		{
			Pillars.RemoveAt(i);
		}
	}
}

