// Fill out your copyright notice in the Description page of Project Settings.


#include "BoidManager.h"

// Sets default values
ABoidManager::ABoidManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    Settings = CreateDefaultSubobject<UBoidSettings>(TEXT("Settings"));
    Target = nullptr;
}

// Called when the game starts or when spawned
void ABoidManager::BeginPlay()
{
	Super::BeginPlay();

    UWorld* World = GetWorld();
    if(World)
    {

        for (int i = 0; i < spawnCount; i++) {
            FVector* pos = new FVector(GetActorLocation() + UKismetMathLibrary::RandomUnitVector() * spawnRadius);
			FRotator* dir = new FRotator((UKismetMathLibrary::RandomUnitVector()).Rotation());
            ABoid* temp = (ABoid*)GetWorld()->SpawnActor(ABoid::StaticClass(), pos, dir);
			temp->Init(Settings, Target);
            temp->DynamicMat = UMaterialInstanceDynamic::Create(temp->BoidMesh->GetMaterial(0), temp);
            temp->BoidMesh->SetMaterial(0, temp->DynamicMat);

            Boids.Add(temp);
            ManagerBoidLocs.Add(temp->GetActorLocation());
        }

        InitBoidLocs();
    }
}

// Called every frame
void ABoidManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (Boids.Num() > 0)
    {
        int NumBoids = Boids.Num();

        float ViewRadius = Settings->PerceptionRadius;
        float AvoidRadius = Settings->AvoidanceRadius;

        ManagerBoidLocs.Empty();

        ParallelFor(NumBoids, [&](int32 Outter)
        {
            Boids[Outter]->NumPerceivedFlockmates = 0; // **********************************
            for (int Index = 0; Index < NumBoids; Index++)
            {
                if (Outter != Index)
                {
                    ABoid* CurrentBoid = Boids[Index];
                    FVector Offset = CurrentBoid->Position - Boids[Outter]->Position;
                    float SqrDst = Offset.X * Offset.X + Offset.Y * Offset.Y + Offset.Z * Offset.Z;

                    if (SqrDst < ViewRadius * ViewRadius) {
                        //FVector CanSee = Offset.GetSafeNormal();
                        //int Angle = UKismetMathLibrary::Acos(FVector::DotProduct(CanSee, Boids[Outter]->Forward));
                        //if (Angle <= Settings->BoidHalfFOVRads)
                        //{
                        Boids[Outter]->NumPerceivedFlockmates += 1;
                        Boids[Outter]->AvgFlockHeading += CurrentBoid->Forward;
                        Boids[Outter]->CentreOfFlockmates += CurrentBoid->Position;
                        //}
                        if (SqrDst < AvoidRadius * AvoidRadius) {
                            Boids[Outter]->AvgAvoidanceHeading -= Offset / SqrDst;
                        }
                    }
                }
            }
        });

        for (int i = 0; i < NumBoids; i++)
        {
            Boids[i]->UpdateBoid(DeltaTime);
            ManagerBoidLocs.Add(Boids[i]->GetActorLocation());
        }
        UpdateBoidLocs();
    }

}