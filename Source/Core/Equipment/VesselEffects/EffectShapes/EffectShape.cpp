// Fill out your copyright notice in the Description page of Project Settings.


#include "EffectShape.h"
#include "../../Vessel.h"
#include "../../../HelperFiles/DefinedDebugHelpers.h"

UEffectShape::UEffectShape()
{
	PrimaryComponentTick.bCanEverTick = true;


	Decal = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal"));
	Decal->SetupAttachment(this);
	Decal->SetVisibility(false);
	Decal->SetHiddenInGame(true);

	EffectShapeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EffectShapeMesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> EffectShapeMeshFinder(TEXT("StaticMesh'/Game/Speegypt/Effects/Meshes/SM_EffectsSphere.SM_EffectsSphere'"));
	if (EffectShapeMeshFinder.Object)
	{
		EffectShapeMesh->SetStaticMesh(EffectShapeMeshFinder.Object);
		EffectShapeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		EffectShapeMesh->SetupAttachment(this);
		EffectShapeMesh->SetUsingAbsoluteScale(true);
		EffectShapeMesh->CastShadow = false;
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> GlowMatFinder(TEXT("MaterialInstanceConstant'/Game/Speegypt/Effects/Materials/MAT_DefaultGlow_Inst.MAT_DefaultGlow_Inst'"));
	if (GlowMatFinder.Object)
	{
		GlowMat = GlowMatFinder.Object;
		if (EffectShapeMesh)
			EffectShapeMesh->SetMaterial(0, GlowMat);
	}
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMatFinder(TEXT("Material'/Game/Speegypt/Effects/Materials/MAT_DefaultDecal.MAT_DefaultDecal'"));
	if (DecalMatFinder.Object)
		DecalMat = DecalMatFinder.Object;
	Decal->SetUsingAbsoluteScale(true);

	Disable();
}

void UEffectShape::Enable()
{
	if (EffectShapeMesh && Decal)
	{
		EffectShapeMesh->SetVisibility(true);
		EffectShapeMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Decal->SetHiddenInGame(false);
	}
	SetComponentTickEnabled(true);
}

void UEffectShape::Disable()
{
	if (EffectShapeMesh && Decal)
	{
		EffectShapeMesh->SetVisibility(false);
		EffectShapeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Decal->SetHiddenInGame(true);
	}
	SetComponentTickEnabled(false);
}

void UEffectShape::StartSwap(bool bStartGrowing)
{
	if (bStartGrowing)
	{
		SwapTimer = 0;
		EffectSwapState = EEffectSwapState::Growing;
	}
	else
	{
		SwapTimer = MaxSwapTimer;
		EffectSwapState = EEffectSwapState::Shrinking;
	}
}

void UEffectShape::Terminate()
{
	if(EffectShapeMesh)
		EffectShapeMesh->DestroyComponent();
	if(Decal)
		Decal->DestroyComponent();
	DestroyComponent();
}

void UEffectShape::BeginPlay()
{
	Super::BeginPlay();

	SetupMesh();

	bBegunPlay = true;
}

void UEffectShape::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bBegunPlay && EffectShapeMesh)
	{
		switch (EffectSwapState)
		{
		case EEffectSwapState::Shrinking:
			EffectShapeMesh->SetWorldScale3D(FVector(FMath::Lerp(MinMeshScale, MaxMeshScale, SwapTimer / MaxSwapTimer)));
			SwapTimer -= DeltaTime;
			if (SwapTimer <= 0)
			{
				EffectSwapState = EEffectSwapState::Growing;
				OnStartGrowing.Broadcast();
			}
			break;
		case EEffectSwapState::Growing:
			EffectShapeMesh->SetWorldScale3D(FVector(FMath::Lerp(MinMeshScale, MaxMeshScale, SwapTimer / MaxSwapTimer)));
			SwapTimer += DeltaTime;
			if (SwapTimer >= MaxSwapTimer)
				EffectSwapState = EEffectSwapState::Idle;
			break;
		default:
			break;
		}
		UpdateDecalVectors();
	}
}

void UEffectShape::SetupMesh()
{
	if (DecalMat && Decal && EffectShapeMesh && GlowMat)
	{
		Decal->SetMaterial(0, DecalMat);
		Decal->CreateDynamicMaterialInstance();

		UMaterialInterface* Mat = Decal->GetMaterial(0);
		UMaterialInstanceDynamic* DynamicMat = Cast<UMaterialInstanceDynamic>(Mat);
		if (DynamicMat)
		{
			DecalDynamicMat = DynamicMat;
			DecalDynamicMat->SetVectorParameterValue("Color", DecalColor);
		}
		Decal->DecalSize = EffectShapeMesh->Bounds.BoxExtent + EffectShapeMesh->Bounds.BoxExtent * 0.25;
		Decal->SetVisibility(true);

		if (EffectShapeMesh)
		{
			EffectShapeMesh->SetMaterial(0, GlowMat);
			MaxMeshScale = EffectShapeMesh->GetComponentScale().X;
		}
	}
}

void UEffectShape::UpdateDecalVectors()
{
	;
}