// Fill out your copyright notice in the Description page of Project Settings.


#include "CapsuleEffectShape.h"

UCapsuleEffectShape::UCapsuleEffectShape()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> EffectShapeMeshFinder(TEXT("StaticMesh'/Game/Speegypt/Effects/Meshes/SM_EffectsCapsule.SM_EffectsCapsule'"));
	if (EffectShapeMeshFinder.Object)
	{
		EffectShapeMesh->SetStaticMesh(EffectShapeMeshFinder.Object);
		EffectShapeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		EffectShapeMesh->SetupAttachment(this);
		TwoToOne = EffectShapeMeshFinder.Object;
	}
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ThreeToOneMeshFinder(TEXT("StaticMesh'/Game/Speegypt/Effects/Meshes/SM_EffectsCapsule_750_250.SM_EffectsCapsule_750_250'"));
	if (ThreeToOneMeshFinder.Object)
	{
		ThreeToOne = ThreeToOneMeshFinder.Object;
	}
	static ConstructorHelpers::FObjectFinder<UStaticMesh> EightToOneMeshFinder(TEXT("StaticMesh'/Game/Speegypt/Effects/Meshes/SM_EffectsCapsule_800_100.SM_EffectsCapsule_800_100'"));
	if (EightToOneMeshFinder.Object)
	{
		EightToOne = EightToOneMeshFinder.Object;
	}
}

void UCapsuleEffectShape::UpdateDecalVectors()
{
	if (DecalDynamicMat && EffectShapeMesh)
	{
		float HalfHeight, Radius;
		switch (MeshType)
		{
		case EEffectCapsuleType::ThreeToOne:
			HalfHeight = 750;
			Radius = 250;
			break;
		case EEffectCapsuleType::EightToOne:
			HalfHeight = 800;
			Radius = 100;
			break;
		default:
			HalfHeight = 1000;
			Radius = 500;
			break;
		}
		float ScaledHeight = HalfHeight * EffectShapeMesh->GetComponentScale().Z;
		float ScaledRadius = Radius * EffectShapeMesh->GetComponentScale().X;
		FVector Start, End;
		FVector Up = GetUpVector();
		Start = GetComponentLocation() + Up * (ScaledHeight - ScaledRadius);
		End = GetComponentLocation() - Up * (ScaledHeight - ScaledRadius);

		DecalDynamicMat->SetScalarParameterValue("Radius", ScaledRadius);
		DecalDynamicMat->SetVectorParameterValue("Start", Start);
		DecalDynamicMat->SetVectorParameterValue("End", End);
	}
}

void UCapsuleEffectShape::SetMeshType(EEffectCapsuleType NewType)
{
	if (EffectShapeMesh)
	{
		MeshType = NewType;
		switch (MeshType)
		{
		case EEffectCapsuleType::TwoToOne:
			EffectShapeMesh->SetStaticMesh(TwoToOne);
			break;
		case EEffectCapsuleType::ThreeToOne:
			EffectShapeMesh->SetStaticMesh(ThreeToOne);
			break;
		case EEffectCapsuleType::EightToOne:
			EffectShapeMesh->SetStaticMesh(EightToOne);
			break;
		}
	}
}

void UCapsuleEffectShape::SetupMesh()
{
	Super::SetupMesh();

	if (DecalDynamicMat)
	{
		DecalDynamicMat->SetScalarParameterValue("bIsBox", 0);
		DecalDynamicMat->SetScalarParameterValue("bIsCapsule", 1);
		DecalDynamicMat->SetScalarParameterValue("bIsCone", 0);
		DecalDynamicMat->SetScalarParameterValue("bIsSphere", 0);
	}
}

#if WITH_EDITOR
void UCapsuleEffectShape::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == "MeshType")
	{
		SetMeshType(MeshType);
	}
}
#endif
