// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Equipment/Vessel.h"
#include "SpeegyptCharacter.generated.h"

class UInputComponent;
class IGrabbableObject;

UCLASS(config=Game)
class ASpeegyptCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class UStaticMeshComponent* VesselMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY()
	UWorld* World;

	UPROPERTY()
	float InteractDist;

	UPROPERTY()
	float InteractRadius;

	UPROPERTY()
	float InteractHoldTimer = 0;

	UPROPERTY()
	bool bIncrimentInteractHoldTimer = false;

	UPROPERTY()
	int Health;

	UPROPERTY()
	float DamageThresh;

	UPROPERTY()
	float DefaultDamageThresh;

	UPROPERTY()
	TEnumAsByte<ECollisionResponse> HeldObjectDefaultCollision;

public:
	ASpeegyptCharacter();

	UPROPERTY(VisibleAnywhere)
	USceneComponent* HeldObjectPosition;

	IGrabbableObject* HeldObject;

protected:
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UVessel* RightVessel;

	/** Returns the current damage threshold of the character */
	UFUNCTION(BlueprintCallable)
	float GetCurrentDamageThresh() { return DamageThresh; };

	/** Returns the current normalized threshold of the character */
	UFUNCTION(BlueprintCallable)
	float GetCurrentNormDamageThresh() { return DamageThresh / DefaultDamageThresh; };

	/** Returns the current health of the character */
	UFUNCTION(BlueprintCallable)
	int GetCurrentHealth() { return Health; };

protected:

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	void OnInteractPressed();

	void OnInteractReleased();

	void OnInteractBegin();

	void OnGrabBegin();
	
	virtual void AimAbility();

	virtual void FireAbility();

	virtual void ResetAbilityAim();

	virtual void ResetAbilityFire();

public:
	void GrabObject(IGrabbableObject* ObjectToGrab);
protected:
	void DropObject();

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

