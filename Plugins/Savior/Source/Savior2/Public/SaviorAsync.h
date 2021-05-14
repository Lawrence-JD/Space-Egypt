//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//			Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Savior2.h"
#include "Savior2_Shared.h"
#include "Runtime/Engine/Classes/Kismet/BlueprintAsyncActionBase.h"

#include "SaviorAsync.generated.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DECLARE_DYNAMIC_DELEGATE(FBeginDATA);
DECLARE_DYNAMIC_DELEGATE_OneParam(FEndDATA,const bool,Success);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FResult);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Save Game World:

UCLASS(meta=(HideThen=true))
class SAVIOR2_API USAVIOR_SaveGameWorld : public UBlueprintAsyncActionBase {
	GENERATED_BODY()
private:
	UPROPERTY() USavior2* Target;
	UPROPERTY() UObject* Outer;
	//
	UFUNCTION() void Execute();
	UFUNCTION() void Finish(const bool Succeeded);
public:
	virtual void Activate() override;
	//
	//
	UPROPERTY(Category = "Event", BlueprintAssignable)
	FResult OnSuccess;
	//
	UPROPERTY(Category = "Event", BlueprintAssignable)
	FResult OnFail;
	//
	//
	/** Saves the whole Game World to Slot.
	Automatically calls 'Write Slot to File' function. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="Context", DisplayName="[SAVIOR] Save Game World (Async)", Keywords="Save World Interface"))
	static USAVIOR_SaveGameWorld* SaveGameWorld(USavior2* Savior, UObject* Context, const bool SaveSlotMeta=true);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Save Game World [+Callbacks]:

UCLASS(meta=(HideThen=true))
class SAVIOR2_API USAVIOR_SaveGameWorld_Callback : public UBlueprintAsyncActionBase {
	GENERATED_BODY()
private:
	UPROPERTY()	FEndDATA FinishedSave;
	UPROPERTY()	FBeginDATA BeganSave;
	UPROPERTY() USavior2* Target;
	UPROPERTY() UObject* Outer;
	//
	UFUNCTION() void Execute();
	UFUNCTION() void Finish(const bool Succeeded);
public:
	virtual void Activate() override;
	//
	//
	UPROPERTY(Category = "Event", BlueprintAssignable)
	FResult OnSuccess;
	//
	UPROPERTY(Category = "Event", BlueprintAssignable)
	FResult OnFail;
	//
	//
	/** Saves the whole Game World to Slot.
	Automatically calls 'Write Slot to File' function.
	@BeganSaveCallback: Custom Delegate Event called when Save begins.
	@FinishedSaveCallback: Custom Delegate Event called when Save completed. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="Context", DisplayName="[SAVIOR] Save Game World [+Callbacks]", Keywords="Save World Interface"))
	static USAVIOR_SaveGameWorld_Callback* SaveGameWorld(USavior2* Savior, UObject* Context, const bool SaveSlotMeta, const FBeginDATA &BeganSaveCallback, const FEndDATA &FinishedSaveCallback);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load Game World:

UCLASS(meta=(HideThen=true))
class SAVIOR2_API USAVIOR_LoadGameWorld : public UBlueprintAsyncActionBase {
	GENERATED_BODY()
private:
	UPROPERTY() USavior2* Target;
	UPROPERTY() UObject* Outer;
	//
	UFUNCTION() void Execute();
	UFUNCTION() void Finish(const bool Succeeded);
public:
	virtual void Activate() override;
	//
	//
	UPROPERTY(Category = "Event", BlueprintAssignable)
	FResult OnSuccess;
	//
	UPROPERTY(Category = "Event", BlueprintAssignable)
	FResult OnFail;
	//
	//
	/** Loads the whole Game World from Slot.
	Automatically calls 'Read Slot from File' function.
	Supports respawn of dynamically created Actors & Components. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="Context", DisplayName="[SAVIOR] Load Game World (Async)", Keywords="Load World Interface"))
	static USAVIOR_LoadGameWorld* LoadGameWorld(USavior2* Savior, UObject* Context);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load Game World [+Callbacks]:

UCLASS(meta=(HideThen=true))
class SAVIOR2_API USAVIOR_LoadGameWorld_Callback : public UBlueprintAsyncActionBase {
	GENERATED_BODY()
private:
	UPROPERTY()	FEndDATA FinishedLoad;
	UPROPERTY()	FBeginDATA BeganLoad;
	UPROPERTY() USavior2* Target;
	UPROPERTY() UObject* Outer;
	//
	UFUNCTION() void Execute();
	UFUNCTION() void Finish(const bool Succeeded);
public:
	virtual void Activate() override;
	//
	//
	UPROPERTY(Category = "Event", BlueprintAssignable)
	FResult OnSuccess;
	//
	UPROPERTY(Category = "Event", BlueprintAssignable)
	FResult OnFail;
	//
	//
	/** Loads the whole Game World from Slot.
	Automatically calls 'Read Slot from File' function.
	Supports respawn of dynamically created Actors & Components.
	@BeganLoadCallback: Custom Delegate Event called when Load begins.
	@FinishedLoadCallback: Custom Delegate Event called when Load completed. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="Context", DisplayName="[SAVIOR] Load Game World [+Callbacks]", Keywords="Load World Interface"))
	static USAVIOR_LoadGameWorld_Callback* LoadGameWorld(USavior2* Savior, UObject* Context, const FBeginDATA &BeganLoadCallback, const FEndDATA &FinishedLoadCallback);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Save Level:

UCLASS(meta=(HideThen=true))
class SAVIOR2_API USAVIOR_SaveLevel : public UBlueprintAsyncActionBase {
	GENERATED_BODY()
private:
	UPROPERTY() USavior2* Target;
	UPROPERTY() UObject* Outer;
	UPROPERTY() FName Level;
	//
	UFUNCTION() void Execute();
	UFUNCTION() void Finish(const bool Succeeded);
public:
	virtual void Activate() override;
	//
	//
	UPROPERTY(Category = "Event", BlueprintAssignable)
	FResult OnSuccess;
	//
	UPROPERTY(Category = "Event", BlueprintAssignable)
	FResult OnFail;
	//
	//
	/** Saves the whole target Level to Slot.
	Automatically calls 'Write Slot to File' function.
	@LevelToSave: The Level Map to be serialized from World. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="Context", DisplayName="[SAVIOR] Save Level (Async)", Keywords="Save Level Interface"))
	static USAVIOR_SaveLevel* SaveLevel(USavior2* Savior, TSoftObjectPtr<UWorld>LevelToSave, UObject* Context, const bool SaveSlotMeta=true);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Save Level [+Callbacks]:

UCLASS(meta=(HideThen=true))
class SAVIOR2_API USAVIOR_SaveLevel_Callback : public UBlueprintAsyncActionBase {
	GENERATED_BODY()
private:
	UPROPERTY()	FEndDATA FinishedSave;
	UPROPERTY()	FBeginDATA BeganSave;
	UPROPERTY() USavior2* Target;
	UPROPERTY() UObject* Outer;
	UPROPERTY() FName Level;
	//
	UFUNCTION() void Execute();
	UFUNCTION() void Finish(const bool Succeeded);
public:
	virtual void Activate() override;
	//
	//
	UPROPERTY(Category = "Event", BlueprintAssignable)
	FResult OnSuccess;
	//
	UPROPERTY(Category = "Event", BlueprintAssignable)
	FResult OnFail;
	//
	//
	/** Saves the whole target Level to Slot.
	Automatically calls 'Write Slot to File' function.
	@LevelToSave: The Level Map to be serialized from World.
	@BeganSaveCallback: Custom Delegate Event called when Save begins.
	@FinishedSaveCallback: Custom Delegate Event called when Save completed. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="Context", DisplayName="[SAVIOR] Save Level [+Callbacks]", Keywords="Save Level Interface"))
	static USAVIOR_SaveLevel_Callback* SaveLevel(USavior2* Savior, TSoftObjectPtr<UWorld>LevelToSave, UObject* Context, const bool SaveSlotMeta, const FBeginDATA &BeganSaveCallback, const FEndDATA &FinishedSaveCallback);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load Level:

UCLASS(meta=(HideThen=true))
class SAVIOR2_API USAVIOR_LoadLevel : public UBlueprintAsyncActionBase {
	GENERATED_BODY()
private:
	UPROPERTY() USavior2* Target;
	UPROPERTY() UObject* Outer;
	UPROPERTY() FName Level;
	//
	UFUNCTION() void Execute();
	UFUNCTION() void Finish(const bool Succeeded);
public:
	virtual void Activate() override;
	//
	//
	UPROPERTY(Category = "Event", BlueprintAssignable)
	FResult OnSuccess;
	//
	UPROPERTY(Category = "Event", BlueprintAssignable)
	FResult OnFail;
	//
	//
	/** Loads the whole target Level to Slot.
	Dynamically created Objects will NOT auto respawn.
	Automatically calls 'Read Slot from File' function.
	@LevelToLoad: The Level Map to be deserialized to World. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="Context", DisplayName="[SAVIOR] Load Level (Async)", Keywords="Load Level Interface"))
	static USAVIOR_LoadLevel* LoadLevel(USavior2* Savior, TSoftObjectPtr<UWorld>LevelToLoad, UObject* Context);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load Level [+Callbacks]:

UCLASS(meta=(HideThen=true))
class SAVIOR2_API USAVIOR_LoadLevel_Callback : public UBlueprintAsyncActionBase {
	GENERATED_BODY()
private:
	UPROPERTY()	FEndDATA FinishedLoad;
	UPROPERTY()	FBeginDATA BeganLoad;
	UPROPERTY() USavior2* Target;
	UPROPERTY() UObject* Outer;
	UPROPERTY() FName Level;
	//
	UFUNCTION() void Execute();
	UFUNCTION() void Finish(const bool Succeeded);
public:
	virtual void Activate() override;
	//
	//
	UPROPERTY(Category = "Event", BlueprintAssignable)
	FResult OnSuccess;
	//
	UPROPERTY(Category = "Event", BlueprintAssignable)
	FResult OnFail;
	//
	//
	/** Loads the whole target Level to Slot.
	Dynamically created Objects will NOT auto respawn.
	Automatically calls 'Read Slot from File' function.
	@LevelToLoad: The Level Map to be deserialized to World.
	@BeganLoadCallback: Custom Delegate Event called when Load begins.
	@FinishedLoadCallback: Custom Delegate Event called when Load completed. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="Context", DisplayName="[SAVIOR] Load Level [+Callbacks]", Keywords="Load Level Interface"))
	static USAVIOR_LoadLevel_Callback* LoadLevel(USavior2* Savior, TSoftObjectPtr<UWorld>LevelToLoad, UObject* Context, const FBeginDATA &BeganLoadCallback, const FEndDATA &FinishedLoadCallback);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Save Game Mode:

UCLASS(meta=(HideThen=true))
class SAVIOR2_API USAVIOR_SaveGameMode : public UBlueprintAsyncActionBase {
	GENERATED_BODY()
private:
	UPROPERTY() USavior2* Target;
	UPROPERTY() UObject* Outer;
	//
	UFUNCTION() void Execute();
	UFUNCTION() void Finish(const bool Succeeded);
public:
	virtual void Activate() override;
	//
	//
	UPROPERTY(Category = "Event", BlueprintAssignable)
	FResult OnSuccess;
	//
	UPROPERTY(Category = "Event", BlueprintAssignable)
	FResult OnFail;
	//
	//
	/** Saves Game Mode Classes to Slot.
	Automatically calls 'Write Slot to File' function. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="Context", DisplayName="[SAVIOR] Save Game Mode (Async)", Keywords="Save Mode Interface"))
	static USAVIOR_SaveGameMode* SaveGameMode(USavior2* Savior, UObject* Context, const bool SaveSlotMeta=true);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Save Game Mode [+Callbacks]:

UCLASS(meta=(HideThen=true))
class SAVIOR2_API USAVIOR_SaveGameMode_Callback : public UBlueprintAsyncActionBase {
	GENERATED_BODY()
private:
	UPROPERTY()	FEndDATA FinishedSave;
	UPROPERTY()	FBeginDATA BeganSave;
	UPROPERTY() USavior2* Target;
	UPROPERTY() UObject* Outer;
	//
	UFUNCTION() void Execute();
	UFUNCTION() void Finish(const bool Succeeded);
public:
	virtual void Activate() override;
	//
	//
	UPROPERTY(Category = "Event", BlueprintAssignable)
	FResult OnSuccess;
	//
	UPROPERTY(Category = "Event", BlueprintAssignable)
	FResult OnFail;
	//
	//
	/** Saves Game Mode Classes to Slot.
	Automatically calls 'Write Slot to File' function.
	@BeganSaveCallback: Custom Delegate Event called when Save begins.
	@FinishedSaveCallback: Custom Delegate Event called when Save completed. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="Context", DisplayName="[SAVIOR] Save Game Mode [+Callbacks]", Keywords="Save Mode Interface"))
	static USAVIOR_SaveGameMode_Callback* SaveGameMode(USavior2* Savior, UObject* Context, const bool SaveSlotMeta, const FBeginDATA &BeganSaveCallback, const FEndDATA &FinishedSaveCallback);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load Game Mode:

UCLASS(meta=(HideThen=true))
class SAVIOR2_API USAVIOR_LoadGameMode : public UBlueprintAsyncActionBase {
	GENERATED_BODY()
private:
	UPROPERTY() USavior2* Target;
	UPROPERTY() UObject* Outer;
	//
	UFUNCTION() void Execute();
	UFUNCTION() void Finish(const bool Succeeded);
public:
	virtual void Activate() override;
	//
	//
	UPROPERTY(Category = "Event", BlueprintAssignable)
	FResult OnSuccess;
	//
	UPROPERTY(Category = "Event", BlueprintAssignable)
	FResult OnFail;
	//
	//
	/** Loads Game Mode Classes from Slot.
	Dynamically created Objects will NOT auto respawn.
	Automatically calls 'Read Slot from File' function. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="Context", DisplayName="[SAVIOR] Load Game Mode (Async)", Keywords="Load Mode Interface"))
	static USAVIOR_LoadGameMode* LoadGameMode(USavior2* Savior, UObject* Context, const bool IgnoreMainPawnTransform);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load Game Mode [+Callbacks]:

UCLASS(meta=(HideThen=true))
class SAVIOR2_API USAVIOR_LoadGameMode_Callback : public UBlueprintAsyncActionBase {
	GENERATED_BODY()
private:
	UPROPERTY()	FEndDATA FinishedLoad;
	UPROPERTY()	FBeginDATA BeganLoad;
	UPROPERTY() USavior2* Target;
	UPROPERTY() UObject* Outer;
	//
	UFUNCTION() void Execute();
	UFUNCTION() void Finish(const bool Succeeded);
public:
	virtual void Activate() override;
	//
	//
	UPROPERTY(Category = "Event", BlueprintAssignable)
	FResult OnSuccess;
	//
	UPROPERTY(Category = "Event", BlueprintAssignable)
	FResult OnFail;
	//
	//
	/** Loads Game Mode Classes from Slot.
	Dynamically created Objects will NOT auto respawn.
	Automatically calls 'Read Slot from File' function.
	@BeganLoadCallback: Custom Delegate Event called when Load begins.
	@FinishedLoadCallback: Custom Delegate Event called when Load completed. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="Context", DisplayName="[SAVIOR] Load Game Mode [+Callbacks]", Keywords="Load Mode Interface"))
	static USAVIOR_LoadGameMode_Callback* LoadGameMode(USavior2* Savior, UObject* Context, const bool IgnoreMainPawnTransform, const FBeginDATA &BeganLoadCallback, const FEndDATA &FinishedLoadCallback);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Save Game Instance:

UCLASS(meta=(HideThen=true))
class SAVIOR2_API USAVIOR_SaveGameInstance : public UBlueprintAsyncActionBase {
	GENERATED_BODY()
private:
	UPROPERTY() USavior2* Target;
	UPROPERTY() UObject* Outer;
	//
	UFUNCTION() void Execute();
	UFUNCTION() void Finish(const bool Succeeded);
public:
	virtual void Activate() override;
	//
	//
	UPROPERTY(Category = "Event", BlueprintAssignable)
	FResult OnSuccess;
	//
	UPROPERTY(Category = "Event", BlueprintAssignable)
	FResult OnFail;
	//
	//
	/** Saves Game Instance Class to Slot.
	Automatically calls 'Write Slot to File' function. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="Context", DisplayName="[SAVIOR] Save Game Instance (Async)", Keywords="Save Instance Interface"))
	static USAVIOR_SaveGameInstance* SaveGameInstance(USavior2* Savior, UObject* Context, const bool SaveSlotMeta=true);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Save Game Instance [+Callbacks]:

UCLASS(meta=(HideThen=true))
class SAVIOR2_API USAVIOR_SaveGameInstance_Callback : public UBlueprintAsyncActionBase {
	GENERATED_BODY()
private:
	UPROPERTY()	FEndDATA FinishedSave;
	UPROPERTY()	FBeginDATA BeganSave;
	UPROPERTY() USavior2* Target;
	UPROPERTY() UObject* Outer;
	//
	UFUNCTION() void Execute();
	UFUNCTION() void Finish(const bool Succeeded);
public:
	virtual void Activate() override;
	//
	//
	UPROPERTY(Category = "Event", BlueprintAssignable)
	FResult OnSuccess;
	//
	UPROPERTY(Category = "Event", BlueprintAssignable)
	FResult OnFail;
	//
	//
	/** Saves Game Instance Class to Slot.
	Automatically calls 'Write Slot to File' function.
	@BeganSaveCallback: Custom Delegate Event called when Save begins.
	@FinishedSaveCallback: Custom Delegate Event called when Save completed. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="Context", DisplayName="[SAVIOR] Save Game Instance [+Callbacks]", Keywords="Save Instance Interface"))
	static USAVIOR_SaveGameInstance_Callback* SaveGameInstance(USavior2* Savior, UObject* Context, const bool SaveSlotMeta, const FBeginDATA &BeganSaveCallback, const FEndDATA &FinishedSaveCallback);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load Game Instance:

UCLASS(meta=(HideThen=true))
class SAVIOR2_API USAVIOR_LoadGameInstance : public UBlueprintAsyncActionBase {
	GENERATED_BODY()
private:
	UPROPERTY() USavior2* Target;
	UPROPERTY() UObject* Outer;
	//
	UFUNCTION() void Execute();
	UFUNCTION() void Finish(const bool Succeeded);
public:
	virtual void Activate() override;
	//
	//
	UPROPERTY(Category = "Event", BlueprintAssignable)
	FResult OnSuccess;
	//
	UPROPERTY(Category = "Event", BlueprintAssignable)
	FResult OnFail;
	//
	//
	/** Loads the Game Instance from Slot.
	Dynamically created Objects will NOT auto respawn.
	Automatically calls 'Read Slot from File' function. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="Context", DisplayName="[SAVIOR] Load Game Instance (Async)", Keywords="Load Instance Interface"))
	static USAVIOR_LoadGameInstance* LoadGameInstance(USavior2* Savior, UObject* Context);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load Game Instance [+Callbacks]:

UCLASS(meta=(HideThen=true))
class SAVIOR2_API USAVIOR_LoadGameInstance_Callback : public UBlueprintAsyncActionBase {
	GENERATED_BODY()
private:
	UPROPERTY()	FEndDATA FinishedLoad;
	UPROPERTY()	FBeginDATA BeganLoad;
	UPROPERTY() USavior2* Target;
	UPROPERTY() UObject* Outer;
	//
	UFUNCTION() void Execute();
	UFUNCTION() void Finish(const bool Succeeded);
public:
	virtual void Activate() override;
	//
	//
	UPROPERTY(Category = "Event", BlueprintAssignable)
	FResult OnSuccess;
	//
	UPROPERTY(Category = "Event", BlueprintAssignable)
	FResult OnFail;
	//
	//
	/** Loads Game Instance from Slot.
	Dynamically created Objects will NOT auto respawn.
	Automatically calls 'Read Slot from File' function.
	@BeganLoadCallback: Custom Delegate Event called when Load begins.
	@FinishedLoadCallback: Custom Delegate Event called when Load completed. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="Context", DisplayName="[SAVIOR] Load Game Instance [+Callbacks]", Keywords="Load Instance Interface"))
	static USAVIOR_LoadGameInstance_Callback* LoadGameInstance(USavior2* Savior, UObject* Context, const FBeginDATA &BeganLoadCallback, const FEndDATA &FinishedLoadCallback);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////