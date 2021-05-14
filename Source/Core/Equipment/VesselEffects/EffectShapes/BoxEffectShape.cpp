// Fill out your copyright notice in the Description page of Project Settings.


#include "BoxEffectShape.h"

UBoxEffectShape::UBoxEffectShape()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> EffectShapeMeshFinder(TEXT("StaticMesh'/Game/Speegypt/Effects/Meshes/SM_EffectsCube.SM_EffectsCube'"));
	if (EffectShapeMeshFinder.Object)
	{
		EffectShapeMesh->SetStaticMesh(EffectShapeMeshFinder.Object);
		EffectShapeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		EffectShapeMesh->SetupAttachment(this);
	}
}

void UBoxEffectShape::SetupMesh()
{
	Super::SetupMesh();

	if (DecalDynamicMat)
	{
		DecalDynamicMat->SetScalarParameterValue("bIsBox", 1);
		DecalDynamicMat->SetScalarParameterValue("bIsCapsule", 0);
		DecalDynamicMat->SetScalarParameterValue("bIsCone", 0);
		DecalDynamicMat->SetScalarParameterValue("bIsSphere", 0);
	}
}

void UBoxEffectShape::UpdateDecalVectors()
{
	if (EffectShapeMesh)
	{
		;
	}
}