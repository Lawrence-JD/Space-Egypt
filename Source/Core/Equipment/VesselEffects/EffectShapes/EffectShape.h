// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/DecalComponent.h"
#include "Components/SceneComponent.h"
#include "EffectShape.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartGrowing);


class UVessel;

UENUM(BlueprintType)
enum class EEffectSwapState : uint8
{
	Idle		UMETA(DisplayName = "Idle"),
	Growing		UMETA(DisplayName = "Growing"),
	Shrinking	UMETA(DisplayName = "Shrinking"),
};

UCLASS()
class SPEEGYPT_API UEffectShape : public USceneComponent
{
public:

	GENERATED_BODY()

	UEffectShape();

	UPROPERTY()
	FOnStartGrowing OnStartGrowing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDecalComponent* Decal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UMaterial* DecalMat;

	UPROPERTY()
	UMaterialInstanceDynamic* DecalDynamicMat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UMaterialInstance* GlowMat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* EffectShapeMesh;

	UPROPERTY()
	FString CollisionProfileName = "Default";

	UPROPERTY()
	FVector DecalColor;

	UPROPERTY()
	EEffectSwapState EffectSwapState = EEffectSwapState::Idle;

	UPROPERTY()
	float SwapTimer = 0;

	UPROPERTY()
	float MaxSwapTimer = 0.5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinMeshScale = 0;

	UPROPERTY()
	float MaxMeshScale;

	virtual void Enable();

	virtual void Disable();

	void StartSwap(bool bStartGrowing = false);

protected:

	UPROPERTY()
	bool bBegunPlay = false;

	void Terminate();

	virtual void BeginPlay() override;

	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void SetupMesh();

	virtual void UpdateDecalVectors();

	friend class UPassiveVesselEffect;
};
