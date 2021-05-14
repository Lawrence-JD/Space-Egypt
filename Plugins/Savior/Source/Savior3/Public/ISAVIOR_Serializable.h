//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//			Copyright 2020 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SaviorTypes.h"
#include "SaviorMetaData.h"

#include "ISAVIOR_Serializable.generated.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UINTERFACE(Category=Synaptech, BlueprintType, meta=(DisplayName="[SAVIOR] Serializable"))
class SAVIOR3_API USAVIOR_Serializable : public UInterface {
	GENERATED_BODY()
};

class SAVIOR3_API ISAVIOR_Serializable {
	GENERATED_BODY()
public:
	/// Save Listener.
	UFUNCTION(Category="Savior 3", BlueprintNativeEvent, meta=(DisplayName="[SAVIOR] On Prepare To Save:", Keywords="Savior Prepare"))
	void OnPrepareToSave();
	//
	/// Load Listener.
	UFUNCTION(Category="Savior 3", BlueprintNativeEvent, meta=(DisplayName="[SAVIOR] On Prepare To Load:", Keywords="Savior Prepare"))
	void OnPrepareToLoad();
	//
	/// Save Listener.
	UFUNCTION(Category="Savior 3", BlueprintNativeEvent, meta=(DisplayName="[SAVIOR] On Saved:", Keywords="Savior Saved"))
	void OnSaved();
	//
	/// Load Listener.
	UFUNCTION(Category="Savior 3", BlueprintNativeEvent, meta=(DisplayName="[SAVIOR] On Loaded:", Keywords="Savior Loaded"))
	void OnLoaded(const FSlotMetaData &MetaData);
	//
	/// Destroy Listener.
	UFUNCTION(Category="Savior 3", BlueprintNativeEvent, meta=(DisplayName="[SAVIOR] On Marked (Auto-Destroy):", Keywords="Savior Prepare"))
	void OnMarkedAutoDestroy();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////