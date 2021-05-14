// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Boid.h"
#include "BoidSettings.h"
#include "Runtime/Core/Public/Async/ParallelFor.h"
#include "Math/UnrealMathUtility.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoidManager.generated.h"

UCLASS()
class SPEEGYPT_API ABoidManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoidManager();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float spawnRadius = 50;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int spawnCount = 300;

	UPROPERTY(EditAnywhere)
	UBoidSettings* Settings;

	UPROPERTY(EditAnywhere)
	AActor* Target;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TArray<ABoid*> Boids;	
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TArray<FVector> ManagerBoidLocs;

	UFUNCTION(BlueprintImplementableEvent)
	void InitBoidLocs();	
	
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateBoidLocs();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
