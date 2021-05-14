// Fill out your copyright notice in the Description page of Project Settings.


#include "AimedVesselEffect.h"
#include "Camera/CameraComponent.h"
#include "OrangeVesselEffect.h"
#include "VioletVesselEffect.h"
#include "Kismet/GameplayStatics.h"
#include "../../../../Character/SpeegyptCharacter.h"

// Sets default values for this component's properties
UAimedVesselEffect::UAimedVesselEffect()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	if (Cast<UOrangeVesselEffect>(this))
		CollisionChannel = ORANGE_CHANNEL;
	else if (Cast<UVioletVesselEffect>(this))
		CollisionChannel = VIOLET_CHANNEL;
}

// Called when the game starts
void UAimedVesselEffect::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld())
	{
		Player = Cast<ASpeegyptCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	}	
}


// Called every frame
void UAimedVesselEffect::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (AbilityState != EVesselAbilityState::Idle)
	{
		if (AbilityState == EVesselAbilityState::Charging)
		{
			if (ChargeAmount >= ChargeMax)
				ChargeAmount = ChargeMax;
			else
				ChargeAmount += ChargeRate * DeltaTime;
		}
		if (Player)
		{
			FVector StartTrace = Player->GetFirstPersonCameraComponent()->GetComponentLocation();
			FVector ForwardVector = Player->GetFirstPersonCameraComponent()->GetForwardVector();
			FVector EndTrace = ((ForwardVector * Range) + StartTrace);
			FCollisionQueryParams TraceParams = FCollisionQueryParams();
			UWorld* World = GetWorld();

			if (World)
			{
				if (World->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, CollisionChannel, TraceParams))
				{
					//SCREENMSG(HitResult.Actor->GetName());
					AActor* Actor = Cast<AActor>(HitResult.Actor);
					if (Actor)
					{
						UActorComponent* Component = Actor->GetComponentByClass(UActiveVesselTarget::StaticClass());
						if (Component)
						{
							if (AbilityState == EVesselAbilityState::Firing)
							{
								ApplyEffect();
								HideReticle();
								if (bRMBDown)
									AbilityState = EVesselAbilityState::Aiming;
								else
									AbilityState = EVesselAbilityState::Idle;
								ChargeAmount = 0;
							}
							else
								DrawReticle();
						}
						else
							HideReticle();
					}
				}
				else
				{
					if (Cast<UVioletVesselEffect>(this))
						DrawReticle();
					else
						HideReticle();
					if (bRMBDown)
						AbilityState = EVesselAbilityState::Aiming;
					else
						AbilityState = EVesselAbilityState::Idle;
				}
			}
		}
	}
}

void UAimedVesselEffect::Aim()
{
	AbilityState = EVesselAbilityState::Aiming;
	bRMBDown = true;
}

void UAimedVesselEffect::Fire()
{
	if(AbilityState == EVesselAbilityState::Aiming)
		AbilityState = EVesselAbilityState::Charging;
}

void UAimedVesselEffect::ResetAim()
{
	AbilityState = EVesselAbilityState::Idle;
	HideReticle();
	bRMBDown = false;
}

void UAimedVesselEffect::ResetFire()
{
	if(AbilityState == EVesselAbilityState::Charging)
		AbilityState = EVesselAbilityState::Firing;
}

void UAimedVesselEffect::ApplyEffect()
{
	;
}

void UAimedVesselEffect::DrawReticle()
{
	;
}


void UAimedVesselEffect::HideReticle()
{
	;
}
