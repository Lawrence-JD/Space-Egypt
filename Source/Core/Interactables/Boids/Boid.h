// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BoidSettings.h" 
#include "BoidHelper.h"
#include "../../HelperFiles/DefinedDebugHelpers.h"

#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Boid.generated.h"

UCLASS()
class SPEEGYPT_API ABoid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoid();

    UPROPERTY()
    UBoidSettings* Settings;

    UPROPERTY()
    UBoidHelper* BoidHelper;

    // State
    UPROPERTY()
    FVector Position;
    UPROPERTY()
    FVector Forward;
    UPROPERTY()
    FVector Velocity;

    // To update:
    UPROPERTY()
    FVector Acceleration;
    UPROPERTY()
    FVector AvgFlockHeading;
    UPROPERTY()
    FVector AvgAvoidanceHeading;
    UPROPERTY()
    FVector CentreOfFlockmates;
    UPROPERTY(VisibleAnywhere)
    int NumPerceivedFlockmates;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh, meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* BoidMesh;

    // Cached
    //Material material;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    UMaterialInstanceDynamic* DynamicMat;

    UPROPERTY()
    FTransform CachedTransform;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* Target;

    UPROPERTY()
    UWorld* World;

	UFUNCTION()
	void Init(UBoidSettings* NewSettings, AActor* NewTarget);

    UFUNCTION()
    void UpdateBoid(float DeltaTime);

    UFUNCTION()
    bool IsHeadingForCollision();

    UFUNCTION()
    FVector ObstacleRays();

    UFUNCTION()
        FVector SteerTowards(FVector Vector);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
