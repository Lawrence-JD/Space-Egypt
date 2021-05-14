// Fill out your copyright notice in the Description page of Project Settings.


#include "VioletVesselTarget.h"
#include "UObject/ConstructorHelpers.h"

#define VIOLET_EFFECT ECC_GameTraceChannel7
#define FIND_SPOT_CHANNEL ECC_GameTraceChannel9

// Sets default values for this component's properties
UVioletVesselTarget::UVioletVesselTarget()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UMaterial> MatFinder(TEXT("Material'/Game/Speegypt/Environment/Materials/MAT_VioletReticle.MAT_VioletReticle'"));
	if (MatFinder.Object)
		RetMat = MatFinder.Object;

	Init();
}

void UVioletVesselTarget::Init()
{
	if (GetOwner())
	{

		DataEntryPrefix = "VioletData_";
		DataType = UVioletTargetData::StaticClass();
		CollisionChannel = VIOLET_CHANNEL;

		PlayerSpawnLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Player Spawn Loaction"));
		
		CaptureCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capture Capsule"));
		CaptureCapsule->SetCapsuleRadius(500);
		CaptureCapsule->SetCapsuleHalfHeight(CaptureCapsule->GetScaledCapsuleRadius());
		CaptureCapsule->SetCollisionProfileName("VioletEffect");

		PlayerSpawnLocation->SetupAttachment(GetOwner()->GetRootComponent());
		CaptureCapsule->SetupAttachment(GetOwner()->GetRootComponent());
	}
	Super::Init();
}

// Called when the game starts
void UVioletVesselTarget::BeginPlay()
{
	Super::BeginPlay();
	for (auto& Component : PrimitiveComponents)
	{
		if (Component->bIsAffectedByThisEffect)
		{
			UPrimitiveComponent* Primitive = GetComponentByName(Component->ComponentName);
			if (Primitive)
			{
				UVioletTargetData* PureComponent = Cast<UVioletTargetData>(Component);
				if(PureComponent)
					PureComponent->BaseMat = Primitive->GetMaterial(0);
				Primitive->SetCollisionResponseToChannel(FIND_SPOT_CHANNEL, ECR_Ignore);
				Primitive->SetCollisionResponseToChannel(VIOLET_EFFECT, ECR_Ignore);
				Primitive->SetMobility(EComponentMobility::Movable);
			}
		}
	}
}


// Called every frame
void UVioletVesselTarget::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UVioletVesselTarget::DrawReticle()
{
	if (Owner)
	{
		for (auto& Component : PrimitiveComponents)
		{
			if (Component && Component->bIsAffectedByThisEffect)
			{
				UPrimitiveComponent* Primitive = GetComponentByName(Component->ComponentName);
				Primitive->SetMaterial(0, RetMat);
			}
		}
	}
}

void UVioletVesselTarget::HideReticle()
{
	if (Owner)
	{
		for (auto& Component : PrimitiveComponents)
		{
			if (Component && Component->bIsAffectedByThisEffect)
			{
				UVioletTargetData* PureComponent = Cast<UVioletTargetData>(Component);
				if (PureComponent)
				{
					UPrimitiveComponent* Primitive = GetComponentByName(PureComponent->ComponentName);
					Primitive->SetMaterial(0, PureComponent->BaseMat);
				}
			}
		}
	}
}
