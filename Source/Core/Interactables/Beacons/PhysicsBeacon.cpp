// Fill out your copyright notice in the Description page of Project Settings.


#include "PhysicsBeacon.h"
#include "PhysicsBeaconSlot.h"

// Sets default values
APhysicsBeacon::APhysicsBeacon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PhysicsMeshFinder(TEXT("StaticMesh'/Game/Speegypt/Environment/Props/SM_Beacon_Physics.SM_Beacon_Physics'"));
	if (PhysicsMeshFinder.Object)
	{
		BeaconMesh->SetStaticMesh(PhysicsMeshFinder.Object);
		BeaconMesh->SetMaterial(0, Vessel->VesselMesh->GetMaterial(0));
		BeaconMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Block); // interact block
		BeaconMesh->SetSimulatePhysics(true);
		BeaconMesh->SetupAttachment(Root);
	}
	Vessel->SetRelativeLocation(FVector(0, 0, 0));
	Vessel->VesselMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Vessel->VesselMesh->SetVisibility(false);
	Vessel->SetupAttachment(BeaconMesh);
	Vessel->SetEffectShapeScale(FVector(0.25));
	Vessel->SetEffectShapeType(EEffectShapeType::Sphere);
}

void APhysicsBeacon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	BeaconMesh->SetWorldScale3D(FVector(0.25));
}

// Called when the game starts or when spawned
void APhysicsBeacon::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APhysicsBeacon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
#if WITH_EDITOR
	if (!HasActorBegunPlay())
		if(BeaconMesh && Vessel && Vessel->VesselMesh)
			BeaconMesh->SetMaterial(0, Vessel->VesselMesh->GetMaterial(0));
#endif
	if (FallingTimer > 0)
	{
		FallingTimer -= DeltaTime;
		if (FallingTimer <= 0)
			BeaconMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	}
}

bool APhysicsBeacon::OnGrabBegin(ASpeegyptCharacter* Player)
{
	EAttachmentRule RuleLocation = EAttachmentRule::SnapToTarget;
	EAttachmentRule RuleRotation = EAttachmentRule::SnapToTarget;
	EAttachmentRule RuleScale = EAttachmentRule::KeepWorld;
	FAttachmentTransformRules Rules = FAttachmentTransformRules(RuleLocation, RuleRotation, RuleScale, true);
	BeaconMesh->SetSimulatePhysics(false);
	BeaconMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BeaconMesh->AttachToComponent(Player->HeldObjectPosition, Rules);
	BeaconMesh->SetWorldScale3D(FVector(0.15, 0.15, 0.15));

	return true;
}

bool APhysicsBeacon::OnGrabEnd(ASpeegyptCharacter* Player)
{
	FHitResult Hit;
	FVector Position = Player->GetFirstPersonCameraComponent()->GetComponentLocation();
	FVector Forward = Player->GetFirstPersonCameraComponent()->GetForwardVector();
	if (GetWorld()->SweepSingleByChannel(Hit,
		Position,
		150 * Forward + Position,
		Player->GetActorRotation().Quaternion(),
		ECollisionChannel::ECC_Visibility,
		FCollisionShape::MakeSphere(BeaconMesh->Bounds.SphereRadius)))
	{
		APhysicsBeaconSlot* Slot = Cast<APhysicsBeaconSlot>(Hit.Actor);
		if (Slot)
		{
			Slot->TakePhysicsBeacon(this);
			return true;
		}
		return false;
	}
	BeaconMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	EDetachmentRule Rule = EDetachmentRule::KeepWorld;
	FDetachmentTransformRules Rules = FDetachmentTransformRules(Rule, true);
	BeaconMesh->DetachFromComponent(Rules);
	BeaconMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BeaconMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	BeaconMesh->SetSimulatePhysics(true);
	BeaconMesh->SetWorldScale3D(FVector(0.25));

	FVector NewPosition = BeaconMesh->GetComponentLocation();
	BeaconMesh->SetWorldLocation(NewPosition + (Player->GetActorForwardVector() * 75));

	BeaconMesh->SetPhysicsLinearVelocity(FVector(0, 0, 0));

	FallingTimer = 1;

	return true;
}

#if WITH_EDITOR
bool APhysicsBeacon::ShouldTickIfViewportsOnly() const
{	// Sets this class up to tick even when the game isnt running.
	return true;
}
#endif
