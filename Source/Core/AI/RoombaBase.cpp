// Fill out your copyright notice in the Description page of Project Settings.


#include "RoombaBase.h"
#include "../Character/SpeegyptCharacter.h"

// Sets default values
ARoombaBase::ARoombaBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh1P"));
	Mesh1P->SetupAttachment(RootComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));
	Mesh1P->SetReceivesDecals(false);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Mesh1P->SetCollisionResponseToChannel(INTERACT_CHANNEL, ECollisionResponse::ECR_Block);

	Vessel = CreateDefaultSubobject<UVessel>(TEXT("Vessel"));
	Vessel->SetRelativeLocation(FVector(60, 20, 60));
	Vessel->VesselMesh->SetupAttachment(Vessel);
	Vessel->VesselMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Vessel->SetupAttachment(Mesh1P);

	// makes it so the effect sphere of the effects is centered on the player, not the vessel in their hand.
	Vessel->Effects.CyanEffect->SetupAttachment(GetCapsuleComponent());
	Vessel->Effects.MagentaEffect->SetupAttachment(GetCapsuleComponent());
	Vessel->Effects.YellowEffect->SetupAttachment(GetCapsuleComponent());
	Vessel->Effects.VioletEffect->SetupAttachment(GetCapsuleComponent());

	Vessel->Effects.CyanEffect->ForceMagnitude = 100;

	Path = CreateDefaultSubobject<USplineComponent>(TEXT("Path"));
	Path->SetAbsolute(true, true, true);

}

// Called when the game starts or when spawned
void ARoombaBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARoombaBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARoombaBase::OnInteract_Implementation(ASpeegyptCharacter* Player)
{
	if (Player)
	{
		EVesselState TempState = Vessel->GetVesselState();
		Vessel->SetVesselState(Player->RightVessel->GetVesselState());
		Player->RightVessel->SetVesselState(TempState);
		LightCooldown = 5.0f;
	}
}