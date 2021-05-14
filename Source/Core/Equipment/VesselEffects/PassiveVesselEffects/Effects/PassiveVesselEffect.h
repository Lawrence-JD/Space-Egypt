// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../EffectShapes/EffectShape.h"
#include "../../EffectShapes/BoxEffectShape.h"
#include "../../EffectShapes/CapsuleEffectShape.h"
#include "../../EffectShapes/ConeEffectShape.h"
#include "../../EffectShapes/SphereEffectShape.h"
#include "../../VesselEffects.h"
#include "../VesselTargets/GroupTargets/GroupVesselTarget.h"
#include "../VesselTargets/PassiveVesselTarget.h"
#include "PassiveVesselEffect.generated.h"

#define INTERACT_CHANNEL ECC_GameTraceChannel2
#define CYAN_CHANNEL     ECC_GameTraceChannel3
#define MAGENTA_CHANNEL  ECC_GameTraceChannel4
#define YELLOW_CHANNEL   ECC_GameTraceChannel5

#define CYAN_BASE_COLOR FVector(0.265, 0.56, 0.847)
#define MAGENTA_BASE_COLOR FVector(0.906, 0.261, 0.947)
#define YELLOW_BASE_COLOR FVector(0.875, 0.93, 0.237)


UCLASS(ABSTRACT)
class SPEEGYPT_API UPassiveVesselEffect : public UVesselEffects
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPassiveVesselEffect();

	virtual void Enable() override;

	virtual void Disable() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UEffectShape* EffectShape;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EEffectShapeType EffectShapeType;

	UPROPERTY()
	UMaterialInstance* GlowMat;

	FVector DecalColor;

	FString CollisionProfileName = "Default";

private:

	UPROPERTY()
	UBoxEffectShape* BoxEffectShape;

	UPROPERTY()
	UCapsuleEffectShape* CapsuleEffectShape;

	UPROPERTY()
	UConeEffectShape* ConeEffectShape;

	UPROPERTY()
	USphereEffectShape* OrbEffectShape;

protected:

	virtual void PostLoad() override;

	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	virtual void ApplyEffect(UPassiveVesselTarget* Target, UStaticMeshComponent* TargetComponent);

	UFUNCTION(BlueprintCallable)
	virtual	void RemoveEffect(UPassiveVesselTarget* Target, UStaticMeshComponent* TargetComponent);

	UFUNCTION(BlueprintCallable)
	virtual void ApplyEffectToGroup(UPassiveVesselTarget* Target, UTargetData* Data);

	UFUNCTION(BlueprintCallable)
	virtual	void RemoveEffectFromGroup(UPassiveVesselTarget* Target, UTargetData* Data);

	void SetEffectShapeType(EEffectShapeType NewType);

public:	

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	//UFUNCTION()
	//void OnOverlapBegin(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	friend class UVessel;
};
