// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"
#include "RockPillar.generated.h"

UCLASS()
class SPEEGYPT_API ARockPillar : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARockPillar();

	UPROPERTY()
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CurrentLifetime = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float SpawnForce = 0;

	UPROPERTY()
	bool bDespawnFlag = false;

	UPROPERTY()
	int DespawnFrameDelay = 3;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	FORCEINLINE bool operator==(const ARockPillar& Other) const { return GetName() == Other.GetName(); }
	FORCEINLINE bool operator>=(const ARockPillar& Other) const { return CurrentLifetime >= Other.CurrentLifetime; }
	FORCEINLINE bool operator<=(const ARockPillar& Other) const { return CurrentLifetime <= Other.CurrentLifetime; }
	FORCEINLINE bool operator>(const ARockPillar& Other) const { return CurrentLifetime > Other.CurrentLifetime; }
	FORCEINLINE bool operator<(const ARockPillar& Other) const { return CurrentLifetime < Other.CurrentLifetime; }

};
