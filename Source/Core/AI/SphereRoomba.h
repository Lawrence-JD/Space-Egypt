// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RoombaBase.h"
#include "GameFramework/Character.h"
#include "SphereRoomba.generated.h"

UCLASS()
class SPEEGYPT_API ASphereRoomba : public ARoombaBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASphereRoomba();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
