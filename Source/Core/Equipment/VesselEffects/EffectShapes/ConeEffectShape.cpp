// Fill out your copyright notice in the Description page of Project Settings.


#include "ConeEffectShape.h"

UConeEffectShape::UConeEffectShape()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> EffectShapeMeshFinder(TEXT("StaticMesh'/Game/Speegypt/Effects/Meshes/SM_EffectsCone.SM_EffectsCone'"));
	if (EffectShapeMeshFinder.Object)
	{
		EffectShapeMesh->SetStaticMesh(EffectShapeMeshFinder.Object);
		EffectShapeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		EffectShapeMesh->SetupAttachment(this);
		EffectShapeMesh->SetUsingAbsoluteRotation(false);
	}
}

void UConeEffectShape::SetupMesh()
{
	Super::SetupMesh();

	if (DecalDynamicMat)
	{
		DecalDynamicMat->SetScalarParameterValue("bIsBox", 0);
		DecalDynamicMat->SetScalarParameterValue("bIsCapsule", 0);
		DecalDynamicMat->SetScalarParameterValue("bIsCone", 1);
		DecalDynamicMat->SetScalarParameterValue("bIsSphere", 0);

		Decal->SetRelativeLocation(FVector(0, 0, -500 * EffectShapeMesh->GetComponentScale().Z));
	}
}

void UConeEffectShape::UpdateDecalVectors()
{
	if (EffectShapeMesh)
	{
		DecalDynamicMat->SetScalarParameterValue("ConeHeight", EffectShapeMesh->GetComponentScale().Z * 1000);
		DecalDynamicMat->SetScalarParameterValue("BaseRadius", EffectShapeMesh->GetComponentScale().X * 500);
		DecalDynamicMat->SetVectorParameterValue("Start", EffectShapeMesh->GetComponentLocation());
		DecalDynamicMat->SetVectorParameterValue("TipToBase", -EffectShapeMesh->GetUpVector());
	}
	
}
