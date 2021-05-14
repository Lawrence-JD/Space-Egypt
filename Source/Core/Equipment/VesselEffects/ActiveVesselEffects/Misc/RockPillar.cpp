// Fill out your copyright notice in the Description page of Project Settings.


#include "RockPillar.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ARockPillar::ARockPillar()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pillar Mesh"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("StaticMesh'/Game/Speegypt/Environment/Meshes/Pillar.Pillar'"));
	if (MeshFinder.Object)
	{
		Mesh->SetStaticMesh(MeshFinder.Object);
		Mesh->SetWorldScale3D(FVector(1, 1, 2));
		Mesh->SetGenerateOverlapEvents(true);
		static ConstructorHelpers::FObjectFinder<UMaterial> MatFinder(TEXT("Material'/Game/Speegypt/Environment/Materials/MAT_RockPillar.MAT_RockPillar'"));
		if (MatFinder.Object)
		{
			Mesh->SetMaterial(0, MatFinder.Object);
		}
	}
}

// Called when the game starts or when spawned
void ARockPillar::BeginPlay()
{
	Super::BeginPlay();
	if(Mesh)
		Mesh->OnComponentBeginOverlap.AddDynamic(this, &ARockPillar::OnOverlapBegin);
}

void ARockPillar::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ARockPillar* PurePillar = Cast<ARockPillar>(OtherComp);
	if (PurePillar)
	{
		if (this > PurePillar)
			Destroy();
	}
}
