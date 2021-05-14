// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "VesselEnums.h"
#include "VesselEffects/VesselEffects.h"
#include "VesselEffects/EmptyVesselEffect.h"
#include "VesselEffects/WhiteVesselEffect.h"
#include "VesselEffects/PassiveVesselEffects/Effects/CyanVesselEffect.h"
#include "VesselEffects/PassiveVesselEffects/Effects/MagentaVesselEffect.h"
#include "VesselEffects/PassiveVesselEffects/Effects/YellowVesselEffect.h"
#include "VesselEffects/ActiveVesselEffects/Effects/LimeVesselEffect.h"
#include "VesselEffects/ActiveVesselEffects/Effects/OrangeVesselEffect.h"
#include "VesselEffects/ActiveVesselEffects/Effects/VioletVesselEffect.h"
#include "VesselEffects/EffectShapes/EffectShape.h"
#include "VesselEffects/EffectShapes/CapsuleEffectShape.h"
#include "Vessel.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FVesselStateChangedDelegate);


/// Struct containing the various effects used by a UVessel. This is done to prevent unintentional garbage collection of these effects.
USTRUCT(BlueprintType)
struct FVesselEffectTypes
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	UEmptyVesselEffect* EmptyEffect;

	UPROPERTY(BlueprintReadOnly)
	UCyanVesselEffect* CyanEffect;

	UPROPERTY(BlueprintReadOnly)
	UMagentaVesselEffect* MagentaEffect;

	UPROPERTY(BlueprintReadOnly)
	UYellowVesselEffect* YellowEffect;

	UPROPERTY(BlueprintReadOnly)
	ULimeVesselEffect* LimeEffect;

	UPROPERTY(BlueprintReadOnly)
	UOrangeVesselEffect* OrangeEffect;

	UPROPERTY(BlueprintReadOnly)
	UVioletVesselEffect* VioletEffect;

	UPROPERTY(BlueprintReadOnly)
	UWhiteVesselEffect* WhiteEffect;
};

/// Class to handle the rendering of vessels, as well as the swapping of vessel effects.
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPEEGYPT_API UVessel : public USceneComponent
{
	GENERATED_BODY()

public:	
	/// Sets up the vessel effects and their attachments, as well as the vessel's mesh and material.
	UVessel();

	/// Mesh used to render this vessel.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* VesselMesh;

	/// Enum used to determine what state the vessel is in, in terms of empty, equipped, cyan, etc. Used when swapping the current effect.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EVesselState VesselState;

	/// Instance of vessel effect container struct.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEffectCapsuleType EffectCapsuleType = EEffectCapsuleType::TwoToOne;

	/// Instance of vessel effect container struct.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVesselEffectTypes Effects;

	/// Polymorphic pointer used to cleanly store which vessel effect is currently active.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UVesselEffects* CurrentVesselEffect;

	UPROPERTY()
	FVesselStateChangedDelegate OnStateChange;

	UPROPERTY(BlueprintReadOnly, Category = "Material")
	UMaterial* EmptyMat;

	UPROPERTY(BlueprintReadOnly, Category = "Material")
	UMaterial* CyanMat;

	UPROPERTY(BlueprintReadOnly, Category = "Material")
	UMaterial* MagentaMat;

	UPROPERTY(BlueprintReadOnly, Category = "Material")
	UMaterial* YellowMat;

	UPROPERTY(BlueprintReadOnly, Category = "Material")
	UMaterial* LimeMat;

	UPROPERTY(BlueprintReadOnly, Category = "Material")
	UMaterial* OrangeMat;

	UPROPERTY(BlueprintReadOnly, Category = "Material")
	UMaterial* VioletMat;

	UPROPERTY(BlueprintReadOnly, Category = "Material")
	UMaterial* WhiteMat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UDecalComponent* Decal;

	UPROPERTY(BlueprintReadOnly, Category = "Material")
	UMaterial* CyanDecalMat;

	UPROPERTY(BlueprintReadOnly, Category = "Material")
	UMaterial* MagentaDecalMat;

	UPROPERTY(BlueprintReadOnly, Category = "Material")
	UMaterial* YellowDecalMat;

	UPROPERTY(BlueprintReadOnly, Category = "Material")
	UMaterialInstanceDynamic* CyanDecalDynamicMat;

	UPROPERTY(BlueprintReadOnly, Category = "Material")
	UMaterialInstanceDynamic* MagentaDecalDynamicMat;

	UPROPERTY(BlueprintReadOnly, Category = "Material")
	UMaterialInstanceDynamic* YellowDecalDynamicMat;

	UFUNCTION()
	void PostLoad() override;

	/// Getter which returns the current Vessel State.
	UFUNCTION(BlueprintCallable)
	EVesselState GetVesselState();

	/// Setter which sets the current vessel state. Automatically changes active material by calling UpdateColor().
	UFUNCTION(BlueprintCallable)
	void SetVesselState(EVesselState NewState);

	/// Used to aim the current effects ability.
	UFUNCTION()
	void AimAbility();

	/// Used to fire the current effects ability.
	UFUNCTION()
	void FireAbility();

	/// Used to register a right click release within the current effect.
	UFUNCTION()
	void ResetAbilityAim();

	/// Used to register a left click release within the current effect.
	UFUNCTION()
	void ResetAbilityFire();

	/// Helper function to update the material based on the current vessel state.
	UFUNCTION(BlueprintCallable)
	void UpdateColor();

	UFUNCTION()
	void SetEffectShapeType(EEffectShapeType NewType);

	UFUNCTION()
	void SetEffectCapsuleType(EEffectCapsuleType NewType);

	UFUNCTION(BlueprintCallable)
	void SetEffectShapeScale(FVector NewScale);

	UFUNCTION()
	void OnShapeStartGrowing();

protected:

	UPROPERTY()
	EVesselState StateToChangeTo;

	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void ChangeState();

#if WITH_EDITOR
	/// Used only for developement, this keeps the vessels material up to date when the state is changed from the editor.
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent);

#endif

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	friend class UEffectShape;
};

