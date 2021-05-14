// Copyright Epic Games, Inc. All Rights Reserved.

#include "SpeegyptCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "../HelperFiles/DefinedDebugHelpers.h"
#include "../Interactables/Interactable.h"
#include "../Interactables/GrabbableObject.h"


DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ASpeegyptCharacter

ASpeegyptCharacter::ASpeegyptCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));
	Mesh1P->SetReceivesDecals(false);

	// Vessel Setup
	RightVessel = CreateDefaultSubobject<UVessel>(TEXT("RightVessel"));
	RightVessel->SetRelativeLocation(FVector(20, 15, 130));
	RightVessel->SetWorldRotation(FRotator(-90, 0, 10));
	RightVessel->VesselMesh->SetupAttachment(RightVessel);
	RightVessel->VesselMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightVessel->SetupAttachment(Mesh1P);

	// makes it so the effect sphere of the effects is centered on the player, not the vessel in their hand.
	RightVessel->Effects.CyanEffect->SetupAttachment(GetCapsuleComponent());
	RightVessel->Effects.MagentaEffect->SetupAttachment(GetCapsuleComponent());
	RightVessel->Effects.YellowEffect->SetupAttachment(GetCapsuleComponent());
	RightVessel->Effects.VioletEffect->SetupAttachment(GetCapsuleComponent());

	RightVessel->Effects.LimeEffect->HitCone->SetupAttachment(this->Mesh1P);

	RightVessel->Effects.CyanEffect->ForceMagnitude = 100;

	RightVessel->SetEffectShapeType(EEffectShapeType::Sphere);

	// Both of these are distances in centimeters for the spherecast used when trying to interact.
	InteractDist = 250;
	InteractRadius = 10;

	// The Speegypt Characters Health and damage threshold
	Health = 3;
	DefaultDamageThresh = 100.0f;
	DamageThresh = 45.0f;

	// Object Holding Setup
	HeldObjectPosition = CreateDefaultSubobject<USceneComponent>(TEXT("HeldObjectPosition"));
	HeldObjectPosition->SetupAttachment(Mesh1P);
	HeldObjectPosition->SetRelativeLocation(FVector(80, 0, 150));
}

void ASpeegyptCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	if (GetWorld())
	{
		World = GetWorld();
	}
	// trying to optimize ticks
	RightVessel->Effects.CyanEffect->Disable();
	RightVessel->Effects.MagentaEffect->Disable();
	RightVessel->Effects.YellowEffect->Disable();
	RightVessel->SetVesselState(RightVessel->GetVesselState());

}

void ASpeegyptCharacter::Tick(float DeltaTime)
{
	if (bIncrimentInteractHoldTimer && InteractHoldTimer < 0.3)
		InteractHoldTimer += DeltaTime;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASpeegyptCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ASpeegyptCharacter::OnInteractPressed);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &ASpeegyptCharacter::OnInteractReleased);
	PlayerInputComponent->BindAction("MouseLeft", IE_Pressed, this, &ASpeegyptCharacter::FireAbility);
	PlayerInputComponent->BindAction("MouseLeft", IE_Released, this, &ASpeegyptCharacter::ResetAbilityFire);
	PlayerInputComponent->BindAction("MouseRight", IE_Pressed, this, &ASpeegyptCharacter::AimAbility);
	PlayerInputComponent->BindAction("MouseRight", IE_Released, this, &ASpeegyptCharacter::ResetAbilityAim);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ASpeegyptCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASpeegyptCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ASpeegyptCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ASpeegyptCharacter::LookUpAtRate);
}

void ASpeegyptCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ASpeegyptCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ASpeegyptCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASpeegyptCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ASpeegyptCharacter::OnInteractPressed()
{
	bIncrimentInteractHoldTimer = true;
}

void ASpeegyptCharacter::OnInteractReleased()
{
	if (InteractHoldTimer >= 0.3)
		OnGrabBegin();
	else
		OnInteractBegin();

	InteractHoldTimer = 0;
	bIncrimentInteractHoldTimer = false;
}

void ASpeegyptCharacter::OnInteractBegin()
{
	FHitResult Hit;
	FVector Position = FirstPersonCameraComponent->GetComponentLocation();
	FVector Forward = FirstPersonCameraComponent->GetForwardVector();
	//DEBUGL(Position, InteractDist * Forward + Position, false); // Debug
	if (World->SweepSingleByChannel(Hit,
		Position,
		InteractDist * Forward + Position,
		GetActorRotation().Quaternion(),
		ECollisionChannel::ECC_GameTraceChannel2, // InteractTrace
		FCollisionShape::MakeSphere(InteractRadius)))
	{
		//SCREENMSG(Hit.Actor->GetName()); // Debug
		IInteractable* PureActor = Cast<IInteractable>(Hit.Actor);
		if(PureActor)
			PureActor->Execute_OnInteract(Cast<AActor>(Hit.Actor), this);
	}
}

void ASpeegyptCharacter::OnGrabBegin()
{
	if (HeldObject)
	{
		DropObject();
		return;
	}

	FHitResult Hit;
	FVector Position = FirstPersonCameraComponent->GetComponentLocation();
	FVector Forward = FirstPersonCameraComponent->GetForwardVector();
	//DEBUGL(Position, InteractDist * Forward + Position, false); // Debug
	if (World->SweepSingleByChannel(Hit,
		Position,
		InteractDist * Forward + Position,
		GetActorRotation().Quaternion(),
		ECollisionChannel::ECC_Visibility, // InteractTrace
		FCollisionShape::MakeSphere(InteractRadius)))
	{
		IGrabbableObject* ObjectToGrab = Cast<IGrabbableObject>(Hit.Actor);
		if (ObjectToGrab)
		{
			GrabObject(ObjectToGrab);
		}
	}
}

void ASpeegyptCharacter::AimAbility()
{
	RightVessel->AimAbility();
}

void ASpeegyptCharacter::FireAbility()
{
	RightVessel->FireAbility();
}

void ASpeegyptCharacter::ResetAbilityAim()
{
	RightVessel->ResetAbilityAim();
}

void ASpeegyptCharacter::ResetAbilityFire()
{
	RightVessel->ResetAbilityFire();
}

void ASpeegyptCharacter::GrabObject(IGrabbableObject* ObjectToGrab)
{
	if (!HeldObject && ObjectToGrab)
	{
		if (ObjectToGrab->OnGrabBegin(this))
			HeldObject = ObjectToGrab;
	}
}

void ASpeegyptCharacter::DropObject()
{
	if (HeldObject)
	{
		if(HeldObject->OnGrabEnd(this))
			HeldObject = nullptr;
	}
}
