// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetData.h"
#include "VesselTarget.generated.h"

/// Parent class for all VesselTarget components. Used extensively to prepared the components of the owning actor for interaction with all Vessel Effects.
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPEEGYPT_API UVesselTarget : public UActorComponent
{
	GENERATED_BODY()

public:	
	/// Simply calls the Init() function. Mainly used when adding component to existing actor.
	UVesselTarget();

	/// Pointer to owning actor. Used to determine if this target has been initialized through the Init() function.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AActor* Owner;

	UPROPERTY()
	TSubclassOf<UTargetData> DataType;

	UPROPERTY()
	FString DataEntryPrefix;

	/// Array of target data objects used to determine behaviors of effect interactions on a per component basis.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UTargetData*> PrimitiveComponents;

	/// Used to filter which effect gets overlap events from the components set to be affected by this target.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TEnumAsByte<ECollisionChannel> CollisionChannel;

	/// Initialization function. Sets the value of Owner, and initializes the PrimitiveComponents array.
	UFUNCTION(BlueprintCallable)
	virtual void Init();

	UFUNCTION(BlueprintCallable)
	virtual void UnInit();

	/// Sets all components of the owning actor to ignore CollisionChannel so that the owning actor won't trigger false positives.
	UFUNCTION()
	void OnComponentDestroyed(bool bDestroyingHierarchy); /// marked for delete

#if WITH_EDITOR
	/// Used to update PrimitiveComponents whenever the owning actor adds or removes a component. Also removes null references in PrimitiveComponents. Components added this way are set to be unaffected by this target.
	/// For developement only.
	virtual void RefreshComponentData();
#endif

	/// Getter for this target's settings related to the given PrimitiveComponent, eturns null if not found
	UFUNCTION(BlueprintCallable)
	virtual UTargetData* GetDataOfComponent(UPrimitiveComponent* Component);

	/// Getter for the PrimitiveComponent of this target's owner of the given name. Used in conjuection with UPassiveTargetData->ComponentName.
	UFUNCTION(BlueprintCallable)
	UPrimitiveComponent* GetComponentByName(FString Name);

	/// Setter for updating the data of the given TargetData. Returns true if successful.
	UFUNCTION(BlueprintCallable)
	bool SetDataOfComponent(UTargetData* Component);

protected:
	/// Called when the game starts, sets up the collision responses for all of the owners components given the data in PrimitiveComponents.
	virtual void BeginPlay() override;

public:	
	/// Calls RefreshComponentData() every frame, but only if running in the editor.
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FORCEINLINE bool operator==(const UVesselTarget& Other) const { return Owner->GetName() == Other.Owner->GetName(); }
};
