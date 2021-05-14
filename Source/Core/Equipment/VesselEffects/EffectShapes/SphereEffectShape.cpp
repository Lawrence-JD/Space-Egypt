// Fill out your copyright notice in the Description page of Project Settings.


#include "SphereEffectShape.h"

USphereEffectShape::USphereEffectShape()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> EffectShapeMeshFinder(TEXT("StaticMesh'/Game/Speegypt/Effects/Meshes/SM_EffectsSphere.SM_EffectsSphere'"));
	if (EffectShapeMeshFinder.Object)
	{
		EffectShapeMesh->SetStaticMesh(EffectShapeMeshFinder.Object);
		EffectShapeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		EffectShapeMesh->SetUsingAbsoluteRotation(true);
		EffectShapeMesh->SetupAttachment(this);
	}
}

void USphereEffectShape::SetupMesh()
{
	Super::SetupMesh();

	if (DecalDynamicMat)
	{
		DecalDynamicMat->SetScalarParameterValue("bIsBox", 0);
		DecalDynamicMat->SetScalarParameterValue("bIsCapsule", 0);
		DecalDynamicMat->SetScalarParameterValue("bIsCone", 0);
		DecalDynamicMat->SetScalarParameterValue("bIsSphere", 1);
	}
}

void USphereEffectShape::UpdateDecalVectors()
{
	if (EffectShapeMesh && DecalDynamicMat)
	{
		DecalDynamicMat->SetScalarParameterValue("Radius", EffectShapeMesh->Bounds.SphereRadius);
		DecalDynamicMat->SetVectorParameterValue("Start", EffectShapeMesh->GetComponentLocation());
		DecalDynamicMat->SetVectorParameterValue("End", EffectShapeMesh->GetComponentLocation());
	}
}
