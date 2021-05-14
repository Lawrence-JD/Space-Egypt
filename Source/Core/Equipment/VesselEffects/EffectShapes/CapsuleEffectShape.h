// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EffectShape.h"
#include "Core/Equipment/VesselEffects/EffectShapes/EffectShape.h"
#include "CapsuleEffectShape.generated.h"


UENUM(BlueprintType)
enum class EEffectCapsuleType : uint8
{
	TwoToOne		UMETA(DisplayName = "Two To One"),
	ThreeToOne		UMETA(DisplayName = "Three To One"),
	EightToOne		UMETA(DisplayName = "Eight To One"),
};

UCLASS()
class SPEEGYPT_API UCapsuleEffectShape : public UEffectShape
{
public:

	GENERATED_BODY()
	
	UCapsuleEffectShape();

	UPROPERTY()
	EEffectCapsuleType MeshType;

	UPROPERTY()
	UStaticMesh* TwoToOne;

	UPROPERTY()
	UStaticMesh* ThreeToOne;

	UPROPERTY()
	UStaticMesh* EightToOne;

	//virtual void BeginPlay() override;

	//void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void UpdateDecalVectors() override;

	void SetMeshType(EEffectCapsuleType NewType);

protected:

	virtual void SetupMesh() override;

#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent);
#endif
};
