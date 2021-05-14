// Fill out your copyright notice in the Description page of Project Settings.


#include "Vessel.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values for this component's properties
UVessel::UVessel()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// Mesh Setup
	VesselMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VesselMesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> VesselMeshFinder(TEXT("StaticMesh'/Game/Speegypt/Environment/Props/SM_CharacterVessel.SM_CharacterVessel'"));
	if (VesselMeshFinder.Object)
		VesselMesh->SetStaticMesh(VesselMeshFinder.Object);

	//VesselMesh->SetWorldScale3D(FVector(0.25, 0.25, 0.25));

	// Material Setup
	static ConstructorHelpers::FObjectFinder<UMaterial> EmptyMatFinder(TEXT("Material'/Game/Speegypt/Environment/Materials/MAT_EmptyVessel.MAT_EmptyVessel'"));
	if (EmptyMatFinder.Object)
	{
		EmptyMat = EmptyMatFinder.Object;
		VesselMesh->SetMaterial(0, EmptyMat);
		VesselMesh->SetReceivesDecals(false);
	}
	static ConstructorHelpers::FObjectFinder<UMaterial> CyanMatFinder(TEXT("Material'/Game/Speegypt/Environment/Materials/MAT_CyanLight.MAT_CyanLight'"));
	if (CyanMatFinder.Object)
		CyanMat = CyanMatFinder.Object;
	static ConstructorHelpers::FObjectFinder<UMaterial> MagentaMatFinder(TEXT("Material'/Game/Speegypt/Environment/Materials/MAT_MagentaLight.MAT_MagentaLight'"));
	if (MagentaMatFinder.Object)
		MagentaMat = MagentaMatFinder.Object;
	static ConstructorHelpers::FObjectFinder<UMaterial> YellowMatFinder(TEXT("Material'/Game/Speegypt/Environment/Materials/MAT_YellowLight.MAT_YellowLight'"));
	if (YellowMatFinder.Object)
		YellowMat = YellowMatFinder.Object;
	static ConstructorHelpers::FObjectFinder<UMaterial> LimeMatFinder(TEXT("Material'/Game/Speegypt/Environment/Materials/MAT_LimeLight.MAT_LimeLight'"));
	if (LimeMatFinder.Object)
		LimeMat = LimeMatFinder.Object;
	static ConstructorHelpers::FObjectFinder<UMaterial> OrangeMatFinder(TEXT("Material'/Game/Speegypt/Environment/Materials/MAT_OrangeLight.MAT_OrangeLight'"));
	if (OrangeMatFinder.Object)
		OrangeMat = OrangeMatFinder.Object;
	static ConstructorHelpers::FObjectFinder<UMaterial> VioletMatFinder(TEXT("Material'/Game/Speegypt/Environment/Materials/MAT_VioletLight.MAT_VioletLight'"));
	if (VioletMatFinder.Object)
		VioletMat = VioletMatFinder.Object;
	static ConstructorHelpers::FObjectFinder<UMaterial> WhiteMatFinder(TEXT("Material'/Game/Speegypt/Environment/Materials/MAT_WhiteLight.MAT_WhiteLight'"));
	if (WhiteMatFinder.Object)
		WhiteMat = WhiteMatFinder.Object;

	// Effect setup
	CurrentVesselEffect = CreateDefaultSubobject<UVesselEffects>(TEXT("CurrentEffect"));
	Effects.EmptyEffect = CreateDefaultSubobject<UEmptyVesselEffect>(TEXT("EmptyEffect"));
	Effects.CyanEffect = CreateDefaultSubobject<UCyanVesselEffect>(TEXT("CyanEffect"));
	Effects.MagentaEffect = CreateDefaultSubobject<UMagentaVesselEffect>(TEXT("MagentaEffect"));
	Effects.YellowEffect = CreateDefaultSubobject<UYellowVesselEffect>(TEXT("YellowEffect"));
	Effects.LimeEffect = CreateDefaultSubobject<ULimeVesselEffect>(TEXT("LimeEffect"));
	Effects.OrangeEffect = CreateDefaultSubobject<UOrangeVesselEffect>(TEXT("OrangeEffect"));
	Effects.VioletEffect = CreateDefaultSubobject<UVioletVesselEffect>(TEXT("VioletEffect"));
	Effects.WhiteEffect = CreateDefaultSubobject<UWhiteVesselEffect>(TEXT("WhiteEffect"));

	CurrentVesselEffect->SetupAttachment(VesselMesh);
	Effects.EmptyEffect->SetupAttachment(VesselMesh);
	Effects.CyanEffect->SetupAttachment(VesselMesh);
	Effects.MagentaEffect->SetupAttachment(VesselMesh);
	Effects.YellowEffect->SetupAttachment(VesselMesh);
	Effects.LimeEffect->SetupAttachment(VesselMesh);
	Effects.OrangeEffect->SetupAttachment(VesselMesh);
	Effects.VioletEffect->SetupAttachment(VesselMesh);
	Effects.WhiteEffect->SetupAttachment(VesselMesh);
	
	// Attaches capsule components from UPassiveVesselEffect to their own corresponding effect. Out of place and weird,
	// ... see if theres a more elegant solution.
	Effects.CyanEffect->EffectShape->SetupAttachment(Effects.CyanEffect);
	Effects.MagentaEffect->EffectShape->SetupAttachment(Effects.MagentaEffect);
	Effects.YellowEffect->EffectShape->SetupAttachment(Effects.YellowEffect);
	Effects.LimeEffect->HitCone->SetupAttachment(Effects.LimeEffect);
	Effects.VioletEffect->CaptureCapsule->SetupAttachment(Effects.VioletEffect);

	Effects.LimeEffect->HitCone->SetRelativeLocation(FVector(210, 80, 0));
	Effects.LimeEffect->HitCone->SetWorldRotation(FRotator(90, 20, 0));
	
	if (GetOwner())
	{
		// this allows these components values to be edited in the details panel of the owning actor.
		GetOwner()->AddInstanceComponent(Effects.CyanEffect);
		GetOwner()->AddInstanceComponent(Effects.MagentaEffect);
		GetOwner()->AddInstanceComponent(Effects.YellowEffect);
		GetOwner()->AddInstanceComponent(Effects.LimeEffect);
		GetOwner()->AddInstanceComponent(Effects.OrangeEffect);
		GetOwner()->AddInstanceComponent(Effects.VioletEffect);
		GetOwner()->AddInstanceComponent(Effects.WhiteEffect);
	}

	VesselState = EVesselState::Empty;
}


// Called when the game starts
void UVessel::BeginPlay()
{
	Super::BeginPlay();

	Effects.CyanEffect->Disable();
	Effects.MagentaEffect->Disable();
	Effects.YellowEffect->Disable();
	Effects.LimeEffect->Disable();
	Effects.OrangeEffect->Disable();
	Effects.VioletEffect->Disable();
	Effects.WhiteEffect->Disable();

	StateToChangeTo = VesselState;
	ChangeState();

	Effects.CyanEffect->EffectShape->OnStartGrowing.AddDynamic(this, &UVessel::OnShapeStartGrowing);
	Effects.MagentaEffect->EffectShape->OnStartGrowing.AddDynamic(this, &UVessel::OnShapeStartGrowing);
	Effects.YellowEffect->EffectShape->OnStartGrowing.AddDynamic(this, &UVessel::OnShapeStartGrowing);
}

// Called every frame
void UVessel::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UVessel::PostLoad()
{
	Super::PostLoad();

#if WITH_EDITOR
	// Ensures the Enum for the Vessel State and the CurrentVesselEffect object are in sync when loading the editor.
	SetVesselState(VesselState);
#endif
}

EVesselState UVessel::GetVesselState()
{
	return VesselState;
}

void UVessel::SetVesselState(EVesselState NewState)
{
	if (VesselState == NewState)
		return;

	StateToChangeTo = NewState;
	UPassiveVesselEffect* PassiveEffect = Cast<UPassiveVesselEffect>(CurrentVesselEffect);
	if (PassiveEffect)
		PassiveEffect->EffectShape->StartSwap();
	else if (VesselState == EVesselState::Empty)
	{
		ChangeState();
		UPassiveVesselEffect* NewPassiveEffect = Cast<UPassiveVesselEffect>(CurrentVesselEffect);
		if (NewPassiveEffect)
			NewPassiveEffect->EffectShape->StartSwap(true);
	}
	else
		ChangeState();
}

void UVessel::ChangeState()
{
	CurrentVesselEffect->Disable();

	VesselState = StateToChangeTo;
	switch (VesselState)
	{
	case EVesselState::Empty:
		CurrentVesselEffect = Effects.EmptyEffect;
		VesselMesh->SetMaterial(0, EmptyMat);
		break;
	case EVesselState::Cyan:
		CurrentVesselEffect = Effects.CyanEffect;
		Cast<UPassiveVesselEffect>(CurrentVesselEffect)->EffectShape->StartSwap(true);
		break;
	case EVesselState::Magenta:
		CurrentVesselEffect = Effects.MagentaEffect;
		Cast<UPassiveVesselEffect>(CurrentVesselEffect)->EffectShape->StartSwap(true);
		break;
	case EVesselState::Yellow:
		CurrentVesselEffect = Effects.YellowEffect;
		Cast<UPassiveVesselEffect>(CurrentVesselEffect)->EffectShape->StartSwap(true);
		break;
	case EVesselState::Lime:
		CurrentVesselEffect = Effects.LimeEffect;
		break;
	case EVesselState::Orange:
		CurrentVesselEffect = Effects.OrangeEffect;
		break;
	case EVesselState::Violet:
		CurrentVesselEffect = Effects.VioletEffect;
		break;
	case EVesselState::White:
		CurrentVesselEffect = Effects.WhiteEffect;
		break;
	}

	CurrentVesselEffect->Enable();
	UpdateColor();

	OnStateChange.Broadcast();
}

void UVessel::AimAbility()
{
	CurrentVesselEffect->Aim();
}

void UVessel::FireAbility()
{
	CurrentVesselEffect->Fire();
}

void UVessel::ResetAbilityAim()
{
	CurrentVesselEffect->ResetAim();
}

void UVessel::ResetAbilityFire()
{
	CurrentVesselEffect->ResetFire();
}

void UVessel::UpdateColor()
{
	switch (VesselState)
	{
	case EVesselState::Cyan:
		if(CyanMat)
			VesselMesh->SetMaterial(0, CyanMat);
		break;
	case EVesselState::Magenta:
		if(MagentaMat)
			VesselMesh->SetMaterial(0, MagentaMat);
		break;
	case EVesselState::Yellow:
		if(YellowMat)
			VesselMesh->SetMaterial(0, YellowMat);
		break;
	case EVesselState::Lime:
		if(LimeMat)
			VesselMesh->SetMaterial(0, LimeMat);
		break;
	case EVesselState::Orange:
		if(OrangeMat)
			VesselMesh->SetMaterial(0, OrangeMat);
		break;
	case EVesselState::Violet:
		if(VioletMat)
			VesselMesh->SetMaterial(0, VioletMat);
		break;
	case EVesselState::White:
		if (WhiteMat)
			VesselMesh->SetMaterial(0, WhiteMat);
		break;
	default:
		break;
	}
}

void UVessel::SetEffectShapeType(EEffectShapeType NewType)
{
	Effects.CyanEffect->SetEffectShapeType(NewType);
	Effects.MagentaEffect->SetEffectShapeType(NewType);
	Effects.YellowEffect->SetEffectShapeType(NewType);
}

void UVessel::SetEffectCapsuleType(EEffectCapsuleType NewType)
{
	UCapsuleEffectShape* CyanCapsule = Cast<UCapsuleEffectShape>(Effects.CyanEffect->EffectShape);
	UCapsuleEffectShape* MagentaCapsule = Cast<UCapsuleEffectShape>(Effects.MagentaEffect->EffectShape);
	UCapsuleEffectShape* YellowCapsule = Cast<UCapsuleEffectShape>(Effects.YellowEffect->EffectShape);

	if (CyanCapsule && MagentaCapsule && YellowCapsule)
	{
		CyanCapsule->SetMeshType(NewType);
		MagentaCapsule->SetMeshType(NewType);
		YellowCapsule->SetMeshType(NewType);
	}
}

void UVessel::SetEffectShapeScale(FVector NewScale)
{
	Effects.CyanEffect->EffectShape->EffectShapeMesh->SetWorldScale3D(NewScale);
	Effects.MagentaEffect->EffectShape->EffectShapeMesh->SetWorldScale3D(NewScale);
	Effects.YellowEffect->EffectShape->EffectShapeMesh->SetWorldScale3D(NewScale);
}

void UVessel::OnShapeStartGrowing()
{
	ChangeState();
}

#if WITH_EDITOR
void UVessel::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	//SCREENMSG(PropertyChangedEvent.GetPropertyName().ToString());

	if (PropertyChangedEvent.GetPropertyName() == "VesselState")
		SetVesselState(VesselState); // this should fix emms bug
	else if (PropertyChangedEvent.GetPropertyName() == "EffectCapsuleType")
		SetEffectCapsuleType(EffectCapsuleType);
}

#endif