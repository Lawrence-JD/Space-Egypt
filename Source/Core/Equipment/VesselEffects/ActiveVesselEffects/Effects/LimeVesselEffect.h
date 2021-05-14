// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HeldVesselEffect.h"
#include "../Targets/LimeVesselTarget.h"
#include "../TargetData/limeTargetData.h"
#include "LimeVesselEffect.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPEEGYPT_API ULimeVesselEffect : public UHeldVesselEffect
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULimeVesselEffect();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* HitCone;

	void Aim() override;

	void Fire() override;

	void ResetAim() override;

	void ResetFire() override;

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void EnableHitCone();

	UFUNCTION()
	void DisableHitCone();

	void ApplyEffect(UActiveVesselTarget* Target, UStaticMeshComponent* TargetComponent);

	void RemoveEffect(UActiveVesselTarget* Target, UStaticMeshComponent* TargetComponent);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
