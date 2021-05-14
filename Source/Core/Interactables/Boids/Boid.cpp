// Fill out your copyright notice in the Description page of Project Settings->


#include "Boid.h"

// Sets default values
ABoid::ABoid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
//	PrimaryActorTick.bCanEverTick = true;

    Settings = CreateDefaultSubobject<UBoidSettings>(TEXT("Settings"));
    BoidHelper = CreateDefaultSubobject<UBoidHelper>(TEXT("Helper"));

    BoidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoidMesh"));
    RootComponent = BoidMesh;

    Settings->SetupAttachment(RootComponent);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> Cone(TEXT("StaticMesh'/Engine/BasicShapes/Cone.Cone'"));
    if (Cone.Object)
    {
        BoidMesh->SetStaticMesh(Cone.Object);
        FRotator temp = GetActorRotation();
        BoidMesh->SetWorldRotation(FRotator(temp.Pitch - 90, temp.Yaw, temp.Roll));
        BoidMesh->SetWorldScale3D(FVector(.2, .2, .2));
		BoidMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        BoidMesh->SetCastShadow(false);
    }

}

// Called when the game starts or when spawned
void ABoid::BeginPlay()
{
	Super::BeginPlay();
	
    CachedTransform = GetActorTransform();
    Position = CachedTransform.GetLocation();
    Forward = GetActorForwardVector();
	CentreOfFlockmates = FVector(0, 0, 0);
	NumPerceivedFlockmates = 0;
    float StartSpeed = (Settings->MinSpeed + Settings->MaxSpeed) / 2;
    Velocity = Forward * StartSpeed;

    World = GetWorld();
}

void ABoid::Init(UBoidSettings* NewSettings, AActor* NewTarget)
{
	Settings = NewSettings;
    Target = NewTarget;
}

// Called every frame
void ABoid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ABoid::UpdateBoid(float DeltaTime) 
{
    Acceleration = FVector(0, 0, 0);

    if (Target) 
    {
        FVector OffsetToTarget = (Target->GetActorLocation() - Position);
        Acceleration = SteerTowards(OffsetToTarget) * Settings->TargetWeight;
    }

    if (NumPerceivedFlockmates != 0)
    {
        CentreOfFlockmates /= NumPerceivedFlockmates;

        FVector OffsetToFlockmatesCentre = (CentreOfFlockmates - Position);

        FVector AlignmentForce = SteerTowards(AvgFlockHeading) * Settings->AlignWeight;
        FVector CohesionForce = SteerTowards(OffsetToFlockmatesCentre) * Settings->CohesionWeight;
        FVector SeperationForce = SteerTowards(AvgAvoidanceHeading) * Settings->SeperateWeight;

        Acceleration += AlignmentForce;
        Acceleration += CohesionForce;
        Acceleration += SeperationForce;
    }
	
    if (IsHeadingForCollision())
    {
		FVector CollisionAvoidDir = ObstacleRays();
        FVector CollisionAvoidForce = SteerTowards(CollisionAvoidDir) * Settings->AvoidCollisionWeight;
        Acceleration += CollisionAvoidForce;
    }

    Velocity += Acceleration * DeltaTime;
    float Speed = Velocity.Size();
    FVector Dir = (Velocity / Speed).GetSafeNormal();
    Speed = FMath::Clamp(Speed, Settings->MinSpeed, Settings->MaxSpeed);
    Velocity = Dir * Speed;

    CachedTransform.SetLocation(CachedTransform.GetLocation() + Velocity * DeltaTime);
	FRotator temp1 = Dir.Rotation();
	FQuat temp2 = FRotator(temp1.Pitch - 90, temp1.Yaw, temp1.Roll).Quaternion();
    CachedTransform.SetRotation(FQuat(temp2.X, temp2.Y, temp2.Z, temp2.W));
    //cachedTransform.forward = dir;
    Position = CachedTransform.GetLocation();
	Forward = Dir;
    SetActorLocationAndRotation(Position, Dir.Rotation()); // BIG RED FLAG
	BoidMesh->SetWorldRotation(FRotator(temp1.Pitch - 90, temp1.Yaw, temp1.Roll));
}

bool ABoid::IsHeadingForCollision() {
    FHitResult hit;
    //DEBUGL(Position, Settings->CollisionAvoidDst * Forward + Position, false);
    if (World->SweepSingleByChannel(hit, Position, Settings->CollisionAvoidDst * Forward + Position, GetActorRotation().Quaternion(), ECollisionChannel::ECC_Visibility, FCollisionShape::MakeSphere(Settings->BoundsRadius))) // BIG RED FLAG
    {
		if (hit.Actor != this)
		{
			return true;
		}
    }
    return false;
}

FVector ABoid::ObstacleRays() 
{
    TArray<FVector> RayDirections = BoidHelper->Directions;

    for (int i = 0; i < RayDirections.Num(); i++) {
		FVector dir = (UKismetMathLibrary::TransformDirection(CachedTransform, RayDirections[i]));
        FHitResult hit;
		//float Angle = UKismetMathLibrary::Acos(FVector::DotProduct(dir, Forward));
		
		//if (Angle <= Settings->BoidHalfFOV)
		//{
			//DEBUGLC(Position, Position + Settings->CollisionAvoidDst * dir, Red, false);
			if (!(World->SweepSingleByChannel(hit, Position, Position + Settings->CollisionAvoidDst * dir, dir.Rotation().Quaternion(), ECollisionChannel::ECC_Visibility, FCollisionShape::MakeSphere(Settings->BoundsRadius)))) // BIG RED FLAG
			{
				//DEBUGLC(Position, Position + Settings->CollisionAvoidDst * dir, Blue, false);
				return dir;
			}
		//}
    }
    return Forward;
}

FVector ABoid::SteerTowards(FVector Vector) 
{
    FVector V = Vector.GetSafeNormal() * Settings->MaxSpeed - Velocity;
    return V.GetClampedToMaxSize(Settings->MaxSteerForce);// FMath::Clamp(V.Size(), V.Size(), Settings->MaxSteerForce);
}