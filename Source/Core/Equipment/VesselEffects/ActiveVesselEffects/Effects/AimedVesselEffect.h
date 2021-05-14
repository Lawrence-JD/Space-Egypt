// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActiveVesselEffect.h"
#include "AimedVesselEffect.generated.h"

class ASpeegyptCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPEEGYPT_API UAimedVesselEffect : public UActiveVesselEffect
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAimedVesselEffect();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FHitResult HitResult;

	UPROPERTY()
	ASpeegyptCharacter* Player;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Range;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float ChargeAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ChargeMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ChargeRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TEnumAsByte<ECollisionChannel> CollisionChannel;

	UPROPERTY()
	bool bRMBDown = false;

	void Aim() override;

	void Fire() override;

	void ResetAim() override;

	void ResetFire() override;

	UFUNCTION()
	virtual void ApplyEffect();

	UFUNCTION()
	virtual void DrawReticle();

	UFUNCTION()
	virtual void HideReticle();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
