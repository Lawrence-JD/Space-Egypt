// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PassiveTargetData.h"
#include "YellowTargetData.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), DefaultToInstanced)
class SPEEGYPT_API UYellowTargetData : public UPassiveTargetData
{
	GENERATED_BODY()

public:

	// If true the component will be hidden while affected, and revealed when unaffected. And vice versa.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Hidden Setting")
	bool bIsAffectedHidden;

	// This needs manually set in the editor, if true the component will simulate physics when affected.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Physics Simulation")
	bool bIsAffectedPhysicsSimulated;

	// This needs manually set in the editor, if true the component will simulate physics when unaffected.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Physics Simulation")
	bool bIsUnaffectedPhysicsSimulated;

	// This changes to the proper collision profile based on the color of the effect and the chosen interaction parameters.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effect Collision Profile Names")
	FName AffectedCollisionProfileName;

	// This changes to the proper collision profile based on the color of the effect and the chosen interaction parameters.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effect Collision Profile Names")
	FName UnaffectedCollisionProfileName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TEnumAsByte<ECollisionResponse> CyanResponse = ECR_Ignore;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TEnumAsByte<ECollisionResponse> MagentaResponse = ECR_Ignore;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TEnumAsByte<ECollisionResponse> LimeResponse = ECR_Ignore;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TEnumAsByte<ECollisionResponse> OrangeResponse = ECR_Ignore;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TEnumAsByte<ECollisionResponse> VioletResponse = ECR_Ignore;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TEnumAsByte<ECollisionResponse> InteractResponse = ECR_Ignore;

};

//USTRUCT(Blueprintable, meta = (BlueprintSpawnableComponent))
//struct FYellowTargetData : public FPassiveTargetData
//{
//	GENERATED_BODY()
//
//public:	
//	// Sets default values for this component's properties
//
//	// If true the component will be hidden while affected, and revealed when unaffected. And vice versa.
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Hidden Setting")
//	bool bIsAffectedHidden;
//
//	// This needs manually set in the editor, if true the component will simulate physics when affected.
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Physics Simulation")
//	bool bIsAffectedPhysicsSimulated;
//
//	// This needs manually set in the editor, if true the component will simulate physics when unaffected.
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Physics Simulation")
//	bool bIsUnaffectedPhysicsSimulated;
//
//	// This changes to the proper collision profile based on the color of the effect and the chosen interaction parameters.
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effect Collision Profile Names")
//	FName AffectedCollisionProfileName;
//
//	// This changes to the proper collision profile based on the color of the effect and the chosen interaction parameters.
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effect Collision Profile Names")
//	FName UnaffectedCollisionProfileName;
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
//	TEnumAsByte<ECollisionResponse> CyanResponse = ECR_Ignore;
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
//	TEnumAsByte<ECollisionResponse> MagentaResponse = ECR_Ignore;
//
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
//	TEnumAsByte<ECollisionResponse> InteractResponse = ECR_Ignore;
//
//	FORCEINLINE bool operator==(const FYellowTargetData& Other) const { return PrimitiveComponent->GetName() == Other.PrimitiveComponent->GetName(); } 
//
//	FORCEINLINE bool operator!=(const FYellowTargetData& Other) const { return PrimitiveComponent->GetName() != Other.PrimitiveComponent->GetName(); } 
//
//	FORCEINLINE bool operator==(const UPrimitiveComponent& Other) const { return PrimitiveComponent->GetName() == Other.GetName(); }
//
//	FORCEINLINE bool operator!=(const UPrimitiveComponent& Other) const { return PrimitiveComponent->GetName() != Other.GetName(); }
//	
//};