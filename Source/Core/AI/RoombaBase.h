// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Equipment/Vessel.h"
#include "../Interactables/Interactable.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
#include "RoombaBase.generated.h"

class ASpeegyptCharacter;

UCLASS()
class SPEEGYPT_API ARoombaBase : public ACharacter, public IInteractable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARoombaBase();

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* Mesh1P;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UVessel* Vessel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USplineComponent* Path;

	UPROPERTY(VisibleAnywhere, BlueprintREadWrite)
	float LightCooldown = 0.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void OnInteract_Implementation(ASpeegyptCharacter* Player) override;

};
