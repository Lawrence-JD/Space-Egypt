// Fill out your copyright notice in the Description page of Project Settings.


#include "Beacon.h"
#include "../../Equipment/VesselEffects/EffectShapes/CapsuleEffectShape.h"
#include "../../HelperFiles/DefinedDebugHelpers.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ABeacon::ABeacon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.5;
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));


	RootComponent = Root;

	// Beacon Mesh Setup
	BeaconMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BeaconMesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BeaconMeshFinder(TEXT("StaticMesh'/Game/Speegypt/Environment/Props/SM_Beacon.SM_Beacon'"));
	if (BeaconMeshFinder.Object)
	{
		BeaconMesh->SetStaticMesh(BeaconMeshFinder.Object);
		BeaconMesh->SetWorldScale3D(FVector(0.5, 0.5, 0.5));
		BeaconMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);
		BeaconMesh->SetupAttachment(Root);
	}

	// Vessel Setup
	Vessel = CreateDefaultSubobject<UVessel>(TEXT("Vessel"));

	Vessel->SetRelativeLocation(FVector(0, 0, 180));
	Vessel->VesselMesh->SetupAttachment(Vessel);
	Vessel->VesselMesh->SetVisibility(false);
	Vessel->SetupAttachment(Root);

	BeaconMesh->SetMaterial(0, Vessel->VesselMesh->GetMaterial(0));

	Vessel->SetVesselState(EVesselState::Empty);

	Vessel->Effects.CyanEffect->ForceMagnitude = 100;
}

void ABeacon::OnVesselColorUpdate()
{
	if (BeaconMesh && Vessel && Vessel->VesselMesh)
		BeaconMesh->SetMaterial(0, Vessel->VesselMesh->GetMaterial(0));
}

// Called when the game starts or when spawned
void ABeacon::BeginPlay()
{
	Super::BeginPlay();

	Vessel->OnStateChange.AddDynamic(this, &ABeacon::OnVesselColorUpdate);
	OnVesselColorUpdate();
}

// Called every frame
void ABeacon::Tick(float DeltaTime)
{	// This ticks even when the game isn't running so that the material updates without having to press play.
	Super::Tick(DeltaTime);

#if WITH_EDITOR
	Vessel->UpdateColor();
	OnVesselColorUpdate();
#endif
}

void ABeacon::OnInteract_Implementation(ASpeegyptCharacter* Player)
{
	if (Player)
	{
		EVesselState TempState = Vessel->GetVesselState();
		Vessel->SetVesselState(Player->RightVessel->GetVesselState());
		Player->RightVessel->SetVesselState(TempState);
	}
}

#if WITH_EDITOR
bool ABeacon::ShouldTickIfViewportsOnly() const
{	// Sets this class up to tick even when the game isnt running.
	return true;
}
#endif
