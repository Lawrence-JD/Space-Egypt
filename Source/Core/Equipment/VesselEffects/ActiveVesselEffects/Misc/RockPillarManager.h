// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RockPillar.h"
#include "RockPillarManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPEEGYPT_API URockPillarManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URockPillarManager();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<ARockPillar*> Pillars;

	UStaticMeshComponent* Reticle;

	UStaticMesh* ReticleMesh;

	UMaterial* ReticleMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxPillars;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxPillarLifetime;

	UFUNCTION(BlueprintCallable)
	bool SpawnPillar(FVector Location, FRotator Rotation, float Force, UPrimitiveComponent* Component = nullptr);

	UFUNCTION(BlueprintCallable)
	void DestroyPillar(ARockPillar* Pillar);

	UFUNCTION(BlueprintCallable)
	void DrawReticle(FVector Location, FRotator Rotation);

	UFUNCTION(BlueprintCallable)
	void HideReticle();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
