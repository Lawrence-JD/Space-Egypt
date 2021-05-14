// Fill out your copyright notice in the Description page of Project Settings.


#include "VioletVesselEffect.h"
#include "../../../../Character/SpeegyptCharacter.h"

#define FIND_SPOT_CHANNEL ECC_GameTraceChannel9

// Sets default values for this component's properties
UVioletVesselEffect::UVioletVesselEffect()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	VioletTargetTracker = CreateDefaultSubobject<UVioletTargetTracker>(TEXT("Violet Target Tracker"));

	CaptureCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capture Capsule"));
	CaptureCapsule->SetCapsuleRadius(500);
	CaptureCapsule->SetCapsuleHalfHeight(CaptureCapsule->GetScaledCapsuleRadius());
	CaptureCapsule->SetCollisionProfileName("VioletEffect");
	Range = 2000;
}

// Called when the game starts
void UVioletVesselEffect::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UVioletVesselEffect::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UVioletVesselEffect::Enable()
{
	Super::Enable();
	CaptureCapsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void UVioletVesselEffect::Disable()
{
	Super::Disable();
	CaptureCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UVioletVesselEffect::DrawReticle()
{
	if (VioletTargetTracker)
	{
		for (auto& Target : VioletTargetTracker->VioletTargets)
		{
			if (Target->Owner && GetOwner())
			{
				if ((Target->Owner->GetActorLocation() - GetOwner()->GetActorLocation()).SizeSquared() < (Range * Range))
					Target->DrawReticle();
				else
					Target->HideReticle();
			}
		}
	}
}

void UVioletVesselEffect::HideReticle()
{
	if (VioletTargetTracker)
	{
		for (auto& Target : VioletTargetTracker->VioletTargets)
		{
			if (Target->Owner)
			{
				Target->HideReticle();
			}
		}
	}
}

void UVioletVesselEffect::ApplyEffect()
{
	AActor* OtherActor = Cast<AActor>(HitResult.Actor);
	UWorld* World = GetWorld();
	if (OtherActor && World && Player)
	{
		UActorComponent* TargetFinder = OtherActor->GetComponentByClass(UVioletVesselTarget::StaticClass());
		if (TargetFinder)
		{
			UVioletVesselTarget* VioletTarget = Cast<UVioletVesselTarget>(TargetFinder);
			if (VioletTarget && VioletTarget->Owner)
			{
				TArray<FVector> MyNewLocations;
				TArray<UPrimitiveComponent*> MyNearComponents;
				TArray<UStaticMeshComponent*> MyComponentsToMove;
				CaptureCapsule->GetOverlappingComponents(MyNearComponents);

				for (auto& MyComponent : MyNearComponents)
				{
					if (MyComponent && Cast<UStaticMeshComponent>(MyComponent) && MyComponent->IsSimulatingPhysics())
					{
						FHitResult MyFinderHit;
						UStaticMeshComponent* MyMesh = Cast<UStaticMeshComponent>(MyComponent);
						FVector MyMin = FVector(0, 0, 0);
						FVector MyMax = FVector(0, 0, 0);
						MyMesh->GetLocalBounds(MyMin, MyMax);
						MyMin *= MyMesh->GetComponentScale();
						MyMax *= MyMesh->GetComponentScale();
						float MyCastRadius = (MyMax.X - MyMin.X) / 2;

						FVector MyFromPlayer = (MyMesh->GetComponentLocation() - Player->GetActorLocation());
						FVector MyDirectionFromPlayer = MyFromPlayer;
						MyDirectionFromPlayer.Normalize();
						float MyDistanceFromPlayer = MyFromPlayer.Size();

						FVector MyStart = VioletTarget->PlayerSpawnLocation->GetComponentLocation() + (MyCastRadius / 2 * VioletTarget->Owner->GetActorUpVector());
						FVector MyEnd = MyStart + (MyDistanceFromPlayer * MyDirectionFromPlayer);

						if (World->SweepSingleByChannel(MyFinderHit,
							MyStart,
							MyEnd,
							MyDirectionFromPlayer.Rotation().Quaternion(),
							FIND_SPOT_CHANNEL, // VioletMoveTrace
							FCollisionShape::MakeBox(FVector(MyCastRadius))))
						{
							if ((MyFinderHit.Location - MyStart).SizeSquared() < (MyCastRadius * MyCastRadius))
								continue; // object cant fit

							MyComponentsToMove.Add(MyMesh);
							MyNewLocations.Add(MyFinderHit.Location);
						}
						else
						{
							MyComponentsToMove.Add(MyMesh);
							MyNewLocations.Add(MyEnd);
						}
					}
				}

				//*********************************************************************************************************************************

				TArray<FVector> OtherNewLocations;
				TArray<UPrimitiveComponent*> OtherNearComponents;
				TArray<UStaticMeshComponent*> OtherComponentsToMove;
				VioletTarget->CaptureCapsule->GetOverlappingComponents(OtherNearComponents);

				for (auto& OtherComponent : OtherNearComponents)
				{
					if (OtherComponent && Cast<UStaticMeshComponent>(OtherComponent) && OtherComponent->IsSimulatingPhysics() && !MyComponentsToMove.Contains(OtherComponent))
					{
						FHitResult OtherFinderHit;
						UStaticMeshComponent* OtherMesh = Cast<UStaticMeshComponent>(OtherComponent);
						FVector OtherMin = FVector(0, 0, 0);
						FVector OtherMax = FVector(0, 0, 0);
						OtherMesh->GetLocalBounds(OtherMin, OtherMax);
						OtherMin *= OtherMesh->GetComponentScale();
						OtherMax *= OtherMesh->GetComponentScale();
						float OtherCastRadius = (OtherMax.X - OtherMin.X) / 2;

						FVector OtherFromComponent = (OtherMesh->GetComponentLocation() - HitResult.Component->GetComponentLocation());
						FVector OtherDirectionFromComponent = OtherFromComponent;
						OtherDirectionFromComponent.Normalize();
						float OtherDistanceFromComponent = OtherFromComponent.Size();

						FVector OtherStart = Player->GetActorLocation();// +(OtherCastRadius * Player->GetActorUpVector());
						FVector OtherEnd = OtherStart + (OtherDirectionFromComponent * OtherDistanceFromComponent);

						if (World->SweepSingleByChannel(OtherFinderHit,
							OtherStart,
							OtherEnd,
							OtherDirectionFromComponent.Rotation().Quaternion(),
							FIND_SPOT_CHANNEL, // VioletMoveTrace
							FCollisionShape::MakeBox(FVector(OtherCastRadius))))
						{
							if ((OtherFinderHit.Location - OtherStart).SizeSquared() < (OtherCastRadius * OtherCastRadius))
								continue; // object cant fit

							OtherComponentsToMove.Add(OtherMesh);
							OtherNewLocations.Add(OtherFinderHit.Location);
						}
						else
						{
							OtherComponentsToMove.Add(OtherMesh);
							OtherNewLocations.Add(OtherEnd);
						}
					}
				}

				//*********************************************************************************************************************************

				for (int i = 0; i < MyNewLocations.Num(); i++)
				{
					MyComponentsToMove[i]->SetWorldLocation(MyNewLocations[i], false, nullptr, ETeleportType::TeleportPhysics);
				}
				for (int i = 0; i < OtherNewLocations.Num(); i++)
				{
					OtherComponentsToMove[i]->SetWorldLocation(OtherNewLocations[i], false, nullptr, ETeleportType::TeleportPhysics);
				}
				FVector Temp = Player->GetActorLocation();
				Player->SetActorLocation(VioletTarget->PlayerSpawnLocation->GetComponentLocation(), false, nullptr, ETeleportType::TeleportPhysics);
				HitResult.Component->SetWorldLocation(Temp, false, nullptr, ETeleportType::TeleportPhysics);
			}
		}
	}
}