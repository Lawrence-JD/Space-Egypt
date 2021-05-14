//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//		Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Reflector.h"
#include "SaviorTypes.h"
#include "SaviorMetaData.h"
#include "ISAVIOR_Procedural.h"
#include "ISAVIOR_Serializable.h"

#include "Runtime/Core/Public/Core.h"
#include "Runtime/Engine/Classes/GameFramework/HUD.h"
#include "Runtime/CoreUObject/Public/Misc/PackageName.h"
#include "Runtime/SlateCore/Public/Fonts/SlateFontInfo.h"
#include "Runtime/Slate/Public/Widgets/Layout/SScaleBox.h"
#include "Runtime/Core/Public/Async/TaskGraphInterfaces.h"
#include "Runtime/Engine/Classes/GameFramework/SaveGame.h"
#include "Runtime/Engine/Classes/GameFramework/GameMode.h"
#include "Runtime/Engine/Classes/GameFramework/GameState.h"
#include "Runtime/Engine/Classes/GameFramework/Character.h"
#include "Runtime/CoreUObject/Public/UObject/SoftObjectPtr.h"
#include "Runtime/Engine/Classes/Components/CapsuleComponent.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"

#include "Savior2.generated.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SerializeLevel"),STAT_FSimpleDelegateGraphTask_SerializeLevel,STATGROUP_TaskGraphTasks);
DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.DeserializeLevel"),STAT_FSimpleDelegateGraphTask_DeserializeLevel,STATGROUP_TaskGraphTasks);
DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SerializeGameMode"),STAT_FSimpleDelegateGraphTask_SerializeGameMode,STATGROUP_TaskGraphTasks);
DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SerializeGameWorld"),STAT_FSimpleDelegateGraphTask_SerializeGameWorld,STATGROUP_TaskGraphTasks);
DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.DeserializeGameMode"),STAT_FSimpleDelegateGraphTask_DeserializeGameMode,STATGROUP_TaskGraphTasks);
DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.DeserializeGameWorld"),STAT_FSimpleDelegateGraphTask_DeserializeGameWorld,STATGROUP_TaskGraphTasks);
DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.RespawnActorFromData"),STAT_FSimpleDelegateGraphTask_RespawnActorFromData,STATGROUP_TaskGraphTasks);
DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SerializeGameInstance"),STAT_FSimpleDelegateGraphTask_SerializeGameInstance,STATGROUP_TaskGraphTasks);
DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.DeserializeGameInstance"),STAT_FSimpleDelegateGraphTask_DeserializeGameInstance,STATGROUP_TaskGraphTasks);
DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.RespawnComponentFromData"),STAT_FSimpleDelegateGraphTask_RespawnComponentFromData,STATGROUP_TaskGraphTasks);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSavior_BeginDataSAVE);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSavior_FinishDataSAVE,const bool,Success);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSavior_BeginDataLOAD);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSavior_FinishDataLOAD,const bool,Success);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Savior 2 Internal Dependencies:

class Savior2;
class USlotMeta;
class TASK_LaunchLoadScreen;
class TASK_SerializeGameWorld;
class TASK_DeserializeGameWorld;

struct FStaticSaviorData {
	TMap<FName,FSaviorMinimal>MinimalData;
	TMap<FName,FSaviorRecord>ComplexData;
	FName Location;
	FName Slot;
	//
	FStaticSaviorData() {
		Location = NAME_None;
		Slot = NAME_None;
		//
		MinimalData.Empty();
		ComplexData.Empty();
	}///
};	static FStaticSaviorData StaticSaviorSlot;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Savior Default Settings:

UCLASS(ClassGroup=Synaptech, Category="Synaptech", config=Game)
class SAVIOR2_API USavior2Settings : public UObject {
	GENERATED_BODY()
	//
	USavior2Settings();
public:
	/// :: SETTINGS ::
	///
	/** Default Player Controller ID. */
	UPROPERTY(Category="General Settings", config, EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0"))
	int32 DefaultPlayerID;
	//
	/** Local Player's Level. If the Game is Progression-based, this is important UI information.
	Many Game genres won't use this; If your Game needs, you have to set value manually before saving. */
	UPROPERTY(Category="General Settings", config, EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0"))
	int32 DefaultPlayerLevel;
	//
	/** Local Player's Alias. 
	Many Game genres won't use this; If your Game needs, you have to set value manually before saving. */
	UPROPERTY(Category="General Settings", config, EditDefaultsOnly, BlueprintReadOnly)
	FString DefaultPlayerName;
	//
	/** Story Chapter Name.
	Many Game genres won't use this; It's mainly for story-driven games. */
	UPROPERTY(Category="General Settings", config, EditDefaultsOnly, BlueprintReadOnly)
	FString DefaultChapter;
	//
	/** Name of Map, Level, that we are saving data from. */
	UPROPERTY(Category="General Settings", config, EditDefaultsOnly, BlueprintReadOnly)
	FString DefaultLocation;
	///
	/// :: PERFORMANCE ::
	///
	/** If enabled, Savior Objects will sort Records corresponding to Actors that aren't present in World.
	If a Record exists in Slot, but doesn't currently exists in World, then a Respawn action is triggered.
	Enabled by default, but disabling this option significantly decreases Loading times.
	This is only relevant for the "Load Game World" function. */
	UPROPERTY(Category="Performance", config, EditDefaultsOnly, BlueprintReadOnly)
	bool RespawnDynamicActors;
	//
	/** If enabled, Savior Objects will sort Records corresponding to Components that aren't present in World.
	If a Record exists in Slot, but doesn't currently exists in World, then a Respawn action is triggered.
	Enabled by default, but disabling this option significantly decreases Loading times.
	This is only relevant for the "Load Game World" function. */
	UPROPERTY(Category="Performance", config, EditDefaultsOnly, BlueprintReadOnly)
	bool RespawnDynamicComponents;
	//
	/** Determines Auto-Instance behaviour for Classes in World.
	Only References children of selected Classes will automatically re-instantiate before loading Properties. */
	UPROPERTY(Category="Reflector", config, EditDefaultsOnly, BlueprintReadOnly)
	TSet<TSubclassOf<UObject>>InstanceScope;
	//
	/** Determines Auto-Respawn behaviour for Classes in World.
	Only References children of selected Classes will automatically respawn before loading Properties. */
	UPROPERTY(Category="Reflector", config, EditDefaultsOnly, BlueprintReadOnly)
	TSet<TSubclassOf<UObject>>RespawnScope;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Savior Core:

UCLASS(ClassGroup=Synaptech, Category="Synaptech", Blueprintable, BlueprintType, meta=(DisplayName="[SAVIOR] Auto-Instanced Object"))
class SAVIOR2_API UAutoInstanced : public UObject, public ISAVIOR_Serializable {
	GENERATED_BODY()
};

UCLASS(ClassGroup=Synaptech, Category="Synaptech", BlueprintType, meta=(DisplayName="[SAVIOR] Slot Data"))
class SAVIOR2_API USlotData : public USaveGame {
	GENERATED_BODY()
	//
	USlotData();
	friend class USavior2;
protected:
	/// :: DATA ::
	///
	/** (Minimal) Object Data Records stored in this Save-Game Instance: [ ID | Record ] */
	UPROPERTY(Category="Data", VisibleAnywhere)
	TMap<FName,FSaviorMinimal>Minimal;
	//
	/** (Complex) Object Data Records stored in this Save-Game Instance: [ ID | Record ] */
	UPROPERTY(Category="Data", VisibleAnywhere)
	TMap<FName,FSaviorRecord>Complex;
public:
	/// :: NATIVE ::
	///
	friend inline FArchive &operator << (FArchive &AR, USlotData &SL) {
		AR << SL.Minimal;
		AR << SL.Complex;
	return AR;}
};

UCLASS(ClassGroup=Synaptech, Category="Synaptech", BlueprintType, meta=(DisplayName="Savior II"))
class SAVIOR2_API USavior2 : public UObject, public Reflector {
	GENERATED_BODY()
	//
	USavior2();
	friend class TASK_LaunchLoadScreen;
	friend class TASK_SerializeGameWorld;
	friend class TASK_DeserializeGameWorld;
private:
	/// :: NATIVE ::
	///
	static float SS_Progress;
	static float SS_Workload;
	static float SS_Complete;
	static float SL_Progress;
	static float SL_Workload;
	static float SL_Complete;
	//
	static EThreadSafety ThreadSafety;
	static EThreadSafety LastThreadState;
	//
	//
	UPROPERTY() UWorld* World;
	///
	/// :: DATA ::
	///
	/** Data Container. */
	UPROPERTY() USlotData* SlotData;
	//
	/** Meta Container. */
	UPROPERTY() USlotMeta* SlotMeta;
protected:
	/// :: NATIVE ::
	///
	/** Load-Screen Timer Handler. */
	FTimerHandle TH_LoadScreen;
	//
	/** Checks if this Object is member of World. */
	virtual bool CheckInstance() const;
	//
	/** Updates Instance's World. */
	virtual void SetWorld(UWorld* InWorld);
	//
	/** Loads Game World from Static Data.
	Triggered by Persistent Level transitions. */
	virtual void StaticLoadGameWorld(UWorld* InWorld);
	//
	/** Setup Slot to perform a Save action. */
	virtual const ESaviorResult PrepareSlotToSave(const UObject* Context);
	//
	/** Setup Slot to perform a Load action. */
	virtual const ESaviorResult PrepareSlotToLoad(const UObject* Context);
	//
	/** Returns Property Name mapped to a Class Version Redirector, if any. */
	virtual FName GetMappedPropertyName(TSubclassOf<UObject> Class, const FName Property) const;
	//
	/** Determines Workload based on Class Filters. */
	virtual float CalculateWorkload() const;
	//
	/** Load-Screen Setup. */
	virtual void LaunchLoadScreen(const EThreadSafety Mode, const FText Info);
	virtual void DestroyLoadScreen();
	//
	const USlotData* GetSlotData() {return SlotData;}
	const USlotMeta* GetSlotMeta() {return SlotMeta;}
	void SetSlotData(USlotData* Data) {SlotData=Data;}
	void SetSlotMeta(USlotMeta* Meta) {SlotMeta=Meta;}
public:
	/// :: NATIVE ::
	///
	bool WriteMetaOnSave;
	bool IgnorePawnTransformOnLoad;
	//
	//
	virtual void ClearWorkload();
	//
	virtual void PostLoad() override;
	virtual void BeginDestroy() override;
	virtual void PostInitProperties() override;
	//
	virtual UWorld* GetWorld() const override;
	///
	/// :: CONFIG ::
	///
	/** Enables Debug Logs. */
	UPROPERTY(Category="Savior", EditAnywhere, BlueprintReadWrite, AdvancedDisplay)
	bool Debug;
	//
	/** Enables Debug Logs within serialization processes. */
	UPROPERTY(Category="Savior", EditAnywhere, BlueprintReadWrite, AdvancedDisplay)
	bool DeepLogs;
	//
	/** Dictates which thread serialization methods are executed within.
	If you're saving and loading many Object Properties, you should consider run the Save-Game in Threaded Mode.
	If you need absolute control of when saving and loading starts and finishes, run the Save-Game in Synchronous Mode (locks Game Thread when loading). */
	UPROPERTY(Category="Savior", EditAnywhere, BlueprintReadWrite)
	ERuntimeMode RuntimeMode;
	//
	/** Dictates which data type this slot will target.
	Complex slots store large information and support auto-respawn of Actors on load;
	Minimal data sets generate smaller files, auto-respawn of Actors is not supported. */
	UPROPERTY(Category="Savior", EditAnywhere, BlueprintReadWrite)
	ERecordType Compression;
	//
	/** Limits serialization actions to determined Classes of Objects in World.
	Only Objects based on selected Classes will have properties saved or loaded.
	Object Classes in Scope are required to implement the 'SAVIOR Serializable' Interface. */
	UPROPERTY(Category="Savior|Scope", EditAnywhere, BlueprintReadWrite)
	TSet<TSubclassOf<UObject>>ObjectScope;
	//
	/** Limits serialization actions to determined Classes of Components in World.
	Only Objects based on selected Classes will have properties saved or loaded. */
	UPROPERTY(Category="Savior|Scope", EditAnywhere, BlueprintReadWrite)
	TSet<TSubclassOf<UActorComponent>>ComponentScope;
	//
	/** Limits serialization actions to determined Classes of Actors in World.
	Only Objects based on selected Classes will have properties saved or loaded. */
	UPROPERTY(Category="Savior|Scope", EditAnywhere, BlueprintReadWrite)
	TSet<TSubclassOf<AActor>>ActorScope;
	///
	/// :: VERSIONING ::
	///
	/** Class Property Versioning. Used as 'Property Name Redirector' for Identity;
	If a Property's name have changed in a Class, Property cannot be loaded from a Save-Game Record using old ID;
	Using a Property Redirector of 'Old Name' mapped to 'New Name' provides a way to load records from outdated Save-Games.
	Only Name Redirectors are supported, type mismatching will generate errors when loading a Property from the Slot. */
	UPROPERTY(Category="Version Control", EditAnywhere)
	TMap<TSubclassOf<UObject>,FSaviorRedirector>Redirectors;
	///
	/// :: LOAD-SCREEN ::
	///
	/** Automatically setup a Load-Screen of selected type when loading. */
	UPROPERTY(Category="Load Screen", EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Screen Mode"))
	ELoadScreenMode LoadScreenMode;
	//
	/** Display Load-Screen when loading, saving or both. */
	UPROPERTY(Category="Load Screen", EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Screen Trigger"))
	ELoadScreenTrigger LoadScreenTrigger;
	//
	/** Text to display with Progress Bar when 'Saving'. */
	UPROPERTY(Category="Load Screen", EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Feedback Text: SAVE"))
	FText FeedbackSAVE;
	//
	/** Text to display with Progress Bar when 'Loading'. */
	UPROPERTY(Category="Load Screen", EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Feedback Text: LOAD"))
	FText FeedbackLOAD;
	//
	/** Minimum time in seconds a Load-Screen HUD should be displayed when loading. */
	UPROPERTY(Category="Load Screen", EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Load-Screen Duration", ClampMin="1.0", UIMin="1.0", ClampMax="100.0", UIMax="100.0"))
	float LoadScreenTimer;
	//
	/** If Load-Screen Mode is a 'Background Blur', dictates how strong the blur frame will be while loading. */
	UPROPERTY(Category="Load Screen", EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Background-Blur Power", ClampMin="1.0", UIMin="1.0", ClampMax="100.0", UIMax="100.0"))
	float BackBlurPower;
	//
	/** Image used to display as background when 'Loading' or 'Saving' if Mode is 'Splash Screen'. */
	UPROPERTY(Category="Load Screen", EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Splash-Screen", AllowedClasses="Texture2D"))
	FSoftObjectPath SplashImage;
	//
	/** Image used to display as background when 'Loading' or 'Saving' if Mode is 'Splash Screen'. */
	UPROPERTY(Category="Load Screen", EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Splash-Movie", AllowedClasses="FileMediaSource"))
	FSoftObjectPath SplashMovie;
	//
	/** If enabled, when Mode is 'Movie Player', Progress Bar will still be displayed on top of video. */
	UPROPERTY(Category="Load Screen", EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Progress-Bar on Movies"))
	bool ProgressBarOnMovie;
	//
	/** If enabled, Threaded Save or Load calls will pause Game Thread. */
	UPROPERTY(Category="Load Screen", EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Pause Game"))
	bool PauseGameOnLoad;
	//
	/** Color tint applied on top of Progress Bar's filler image. */
	UPROPERTY(Category="Load Screen", EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta=(DisplayName="Progress-Bar Tint"))
	FLinearColor ProgressBarTint;
	//
	/* Stretching mode of Splash Image, if applicable. */
	UPROPERTY(Category="Load Screen", EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta=(DisplayName="Splash Stretch-Mode"))
	TEnumAsByte<EStretch::Type> SplashStretch;
	//
	/** Font used to display 'Loading' or 'Saving' feedback text. */
	UPROPERTY(Category="Load Screen", EditAnywhere, BlueprintReadWrite, AdvancedDisplay, meta=(DisplayName="Feedback Font"))
	FSlateFontInfo FeedbackFont;
	///
	/// :: UI/UX ::
	///
	/** Name for the (*.sav) file stored on disk.
	If no name provided, by default Slot Asset's name is used. */
	UPROPERTY(Category="UI/UX", EditDefaultsOnly, BlueprintReadWrite)
	FText SlotFileName;
	//
	/** Thumbnail Image used to display as Screenshot when this Object is displayed on UI/UX Elements. */
	UPROPERTY(Category="UI/UX", BlueprintReadWrite, meta=(AllowedClasses="Texture2D"))
	FSoftObjectPath SlotThumbnail;
	//
	/** List of Texture Assets used as thumbnail for each corresponding Level.
	When a Slot is saved within a Level, Thumbnail will be added to Slot.
	These Textures are only loaded to memory when requested by UI. */
	UPROPERTY(Category="UI/UX", EditDefaultsOnly, BlueprintReadWrite)
	TMap<TSoftObjectPtr<UWorld>,TSoftObjectPtr<UTexture2D>> LevelThumbnails;
	///
	/// :: EVENTS ::
	///
	/** Blueprint Assignable Event: On Data Save. */
	UPROPERTY(Category = "Events", BlueprintAssignable, meta = (DisplayName = "[SAVIOR] On Begin Data Save:"))
	FSavior_BeginDataSAVE EVENT_OnBeginDataSAVE;
	//
	/** Blueprint Assignable Event: On Finish Data Save. */
	UPROPERTY(Category = "Events", BlueprintAssignable, meta = (DisplayName = "[SAVIOR] On Finish Data Save:"))
	FSavior_FinishDataSAVE EVENT_OnFinishDataSAVE;
	//
	/** Blueprint Assignable Event: On Data Load. */
	UPROPERTY(Category = "Events", BlueprintAssignable, meta = (DisplayName = "[SAVIOR] On Begin Data Load:"))
	FSavior_BeginDataLOAD EVENT_OnBeginDataLOAD;
	//
	/** Blueprint Assignable Event: On Finish Data Load. */
	UPROPERTY(Category = "Events", BlueprintAssignable, meta = (DisplayName = "[SAVIOR] On Finish Data Load:"))
	FSavior_FinishDataLOAD EVENT_OnFinishDataLOAD;
	///
	/// :: FUNCTIONS ::
	///
	/** Writes all Data stored in this 'SaveGame' Object into a '.sav' file.
	@PlayerID: ID of Player Controller for recording. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Write Slot to File (.SAV)", ExpandEnumAsExecs="Result"))
	void WriteSlotToFile(const int32 PlayerID, ESaviorResult &Result);
	//
	/** Reads all Data from the '.sav' file into this 'SaveGame' Object.
	@PlayerID: ID of Player Controller in Slot. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Read Slot from File (.SAV)", ExpandEnumAsExecs="Result"))
	void ReadSlotFromFile(const int32 PlayerID, ESaviorResult &Result);
	//
	/** Deletes this 'SaveGame' Object's '.sav' file if exists.
	@PlayerID: ID of Player Controller in Slot. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Delete Slot's File (.SAV)", ExpandEnumAsExecs="Result"))
	void DeleteSlotFile(const int32 PlayerID, ESaviorResult &Result);
	//
	/** Checks if this 'SaveGame' Object's '.sav' file exists. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Find Slot's File (.SAV)", ExpandEnumAsExecs="Result"))
	void FindSlotFile(ESaviorResult &Result);
	//
	/** Saves Slot's Meta-Data to File (*.sav).
	Game Object's data will NOT be serialized. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Save Slot Meta-Data (.SAV)", ExpandEnumAsExecs="Result"))
	void SaveSlotMetaData(ESaviorResult &Result);
	//
	/** Loads Slot's Meta-Data from File (*.sav).
	Game Object's data will NOT be serialized. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Slot Meta-Data (.SAV)", ExpandEnumAsExecs="Result"))
	void LoadSlotMetaData(ESaviorResult &Result);
	//
	/** Get Reference to the Meta-Data Object from this Slot. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Get Slot Meta-Data"))
	USlotMeta* GetSlotMetaData() const;
	//
	//
	/** Saves to Slot all Properties marked 'SaveGame' of Target Object.
	This is NOT a call to 'Write Slot to File' function, call it manually.
	@Object: Object Reference Properties are read from. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Save Object", ExpandEnumAsExecs="Result"))
	void SaveObject(UObject* Object, ESaviorResult &Result);
	//
	/** Loads from Slot all Properties marked 'SaveGame' of Target Object.
	This is NOT a call to 'Read Slot from File' function, call it manually.
	@Object: Object Reference loaded Properties are written to. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Object", ExpandEnumAsExecs="Result"))
	void LoadObject(UObject* Object, ESaviorResult &Result);
	//
	//
	/** Saves to Slot all Properties marked 'SaveGame' of Target Object and its Children.
	This is NOT a call to 'Write Slot to File' function, call it manually.
	@Object: Root Object Reference Properties are read from. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Save Object Hierarchy", ExpandEnumAsExecs="Result"))
	void SaveObjectHierarchy(UObject* Object, ESaviorResult &Result);
	//
	/** Loads from Slot all Properties marked 'SaveGame' of Target Object and its Children.
	This is NOT a call to 'Read Slot from File' function, call it manually.
	@Object: Root Object Reference loaded Properties are written to. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Object Hierarchy", ExpandEnumAsExecs="Result"))
	void LoadObjectHierarchy(UObject* Object, ESaviorResult &Result);
	//
	//
	/** Saves to Slot all Properties marked 'SaveGame' of Target Object Class.
	This is NOT a call to 'Write Slot to File' function, call it manually.
	@Object: Root Object Reference Properties are read from. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Save Objects of Class", ExpandEnumAsExecs="Result"))
	void SaveObjectsOfClass(TSubclassOf<UObject>Class, const bool SaveHierarchy, ESaviorResult &Result);
	//
	/** Loads from Slot all Properties marked 'SaveGame' of Target Object Class.
	This is NOT a call to 'Read Slot from File' function, call it manually.
	@Object: Root Object Reference loaded Properties are written to. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Objects of Class", ExpandEnumAsExecs="Result"))
	void LoadObjectsOfClass(TSubclassOf<UObject>Class, const bool LoadHierarchy, ESaviorResult &Result);
	//
	//
	/** Saves to Slot all Properties marked 'SaveGame' of Target Component.
	This is NOT a call to 'Write Slot to File' function, call it manually.
	@Component: Object Reference Properties are read from. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Save Component", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void SaveComponent(UObject* Context, UActorComponent* Component, ESaviorResult &Result);
	//
	/** Loads from Slot all Properties marked 'SaveGame' of Target Component.
	This is NOT a call to 'Read Slot from File' function, call it manually.
	@Component: Object Reference loaded Properties are written to. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Component", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void LoadComponent(UObject* Context, UActorComponent* Component, ESaviorResult &Result);
	//
	/** Loads from Slot all Properties marked 'SaveGame' of Component from given 'SGUID'.
	Only COMPLEX data set store SGUID info. Minimal set will always fail this.
	This is NOT a call to 'Read Slot from File' function, call it manually.
	@SGUID: Target Component must contain a 'SGUID' Property of type FGuid marked 'SaveGame'. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Component by GUID", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void LoadComponentWithGUID(UObject* Context, const FGuid &SGUID, ESaviorResult &Result);
	//
	//
	/** Saves to Slot all Properties marked 'SaveGame' of Target Actor.
	This is NOT a call to 'Write Slot to File' function, call it manually.
	@Actor: Object Reference Properties are read from. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Save Actor", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void SaveActor(UObject* Context, AActor* Actor, ESaviorResult &Result);
	//
	/** Loads from Slot all Properties marked 'SaveGame' of Target Actor.
	This is NOT a call to 'Read Slot from File' function, call it manually.
	@Actor: Object Reference loaded Properties are written to. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Actor", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void LoadActor(UObject* Context, AActor* Actor, ESaviorResult &Result);
	//
	/** Loads from Slot all Properties marked 'SaveGame' of Actor from given 'SGUID'.
	Only COMPLEX data set store SGUID info. Minimal set will always fail this.
	This is NOT a call to 'Read Slot from File' function, call it manually.
	@SGUID: Target Actor must contain a 'SGUID' Property of type FGuid marked 'SaveGame'. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Actor by GUID", WorldContext="Context", ExpandEnumAsExecs="Result"))
	AActor* LoadActorWithGUID(UObject* Context, const FGuid &SGUID, ESaviorResult &Result);
	//
	//
	/** Saves to Slot all Properties marked 'SaveGame' of Target Actor and its Children.
	This is NOT a call to 'Write Slot to File' function, call it manually.
	@Actor: Object Reference Properties are read from. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Save Actor Hierarchy", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void SaveActorHierarchy(UObject* Context, AActor* Actor, ESaviorResult &Result);
	//
	/** Loads from Slot all Properties marked 'SaveGame' of Target Actor and its Children.
	This is NOT a call to 'Read Slot from File' function, call it manually.
	@Actor: Object Reference loaded Properties are written to. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Actor Hierarchy", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void LoadActorHierarchy(UObject* Context, AActor* Actor, ESaviorResult &Result);
	//
	/** Loads from Slot all Properties marked 'SaveGame' of Actor and its children from given 'SGUID'.
	Only COMPLEX data set store SGUID info. Minimal set will always fail this.
	This is NOT a call to 'Read Slot from File' function, call it manually.
	@SGUID: Target Actor must contain a 'SGUID' Property of type FGuid marked 'SaveGame'. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Actor Hierarchy by GUID", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void LoadActorHierarchyWithGUID(UObject* Context, const FGuid &SGUID, ESaviorResult &Result);
	//
	//
	/** Saves to Slot all Properties marked 'SaveGame' of Target Actor Class.
	This is NOT a call to 'Write Slot to File' function, call it manually.
	@Actor: Object Reference Properties are read from. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Save Actors of Class", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void SaveActorsOfClass(UObject* Context, TSubclassOf<AActor>Class, const bool SaveHierarchy, ESaviorResult &Result);
	//
	/** Loads from Slot all Properties marked 'SaveGame' of Target Actor Class.
	This is NOT a call to 'Read Slot from File' function, call it manually.
	@Actor: Object Reference loaded Properties are written to. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Actors of Class", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void LoadActorsOfClass(UObject* Context, TSubclassOf<AActor>Class, const bool LoadHierarchy, ESaviorResult &Result);
	//
	//
	/** Saves to Slot all Properties marked 'SaveGame' of defined Game Instance Class.
	This is NOT a call to 'Write Slot to File' function, call it manually.
	@Instance: GI Reference Properties are read from. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Save GI Singleton", ExpandEnumAsExecs="Result"))
	void SaveGameInstanceSingleTon(UGameInstance* Instance, ESaviorResult &Result);
	//
	//
	/** Loads from Slot all Properties marked 'SaveGame' of defined Game Instance Class.
	This is NOT a call to 'Read Slot from File' function, call it manually.
	@Instance: GI Reference loaded Properties are written to. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load GI Singleton", ExpandEnumAsExecs="Result"))
	void LoadGameInstanceSingleTon(UGameInstance* Instance, ESaviorResult &Result);
	//
	//
	/** Saves the whole Game World to Slot.
	Automatically calls 'Write Slot to File' function. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Save Game World", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void SaveGameWorld(UObject* Context, ESaviorResult &Result);
	//
	/** Loads the whole Game World from Slot.
	Automatically calls 'Read Slot from File' function.
	(Complex) supports respawn of dynamically created Actors & Components. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Game World", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void LoadGameWorld(UObject* Context, ESaviorResult &Result);
	//
	//
	/** Saves the whole target Level to Slot.
	Automatically calls 'Write Slot to File' function.
	@LevelToSave: The Name of Level to be serialized. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Save Level", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void SaveLevel(UObject* Context, FName LevelToSave, ESaviorResult &Result);
	//
	/** Loads the whole target Level from Slot.
	Dynamically created Objects will NOT auto respawn.
	Automatically calls 'Read Slot from File' function.
	@LevelToLoad: The Name of Level to be deserialized. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Level", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void LoadLevel(UObject* Context, FName LevelToLoad, ESaviorResult &Result);
	//
	//
	/** Saves the Game-Mode Classes to Slot.
	Automatically calls 'Write Slot to File' function. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Save Game Mode", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void SaveGameMode(UObject* Context, ESaviorResult &Result);
	//
	/** Loads Game-Mode Classes from Slot.
	Dynamically created Objects will NOT auto respawn.
	Automatically calls 'Read Slot from File' function. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Game Mode", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void LoadGameMode(UObject* Context, ESaviorResult &Result);
	//
	//
	/** Saves World's Game-Instance to Slot.
	Automatically calls 'Write Slot to File' function. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Save Game Instance", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void SaveGameInstance(UObject* Context, ESaviorResult &Result);
	//
	/** Loads World's Game-Instance from Slot.
	Dynamically created Objects will NOT auto respawn.
	Automatically calls 'Read Slot from File' function. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Game Instance", WorldContext="Context", ExpandEnumAsExecs="Result"))
	void LoadGameInstance(UObject* Context, ESaviorResult &Result);
	//
	//
	/** Generates (Complex) Object Data-Record to be stored in this Slot.
	@Object: Target Object for Code Reflection Analysis. */
	UFUNCTION(Category="Savior 2", meta=(DisplayName="[SAVIOR] Generate (Complex) Record : OBJECT", BlueprintInternalUseOnly=true))
	FSaviorRecord GenerateRecord_Object(const UObject* Object);
	//
	/** Generates (Complex) Component Data-Record to be stored in this Slot.
	@Component: Target Component for Code Reflection Analysis. */
	UFUNCTION(Category="Savior 2", meta=(DisplayName="[SAVIOR] Generate (Complex) Record : COMPONENT", BlueprintInternalUseOnly=true))
	FSaviorRecord GenerateRecord_Component(const UActorComponent* Component);
	//
	/** Generates (Complex) Actor Data-Record to be stored in this Slot.
	@Actor: Target Actor for Code Reflection Analysis. */
	UFUNCTION(Category="Savior 2", meta=(DisplayName="[SAVIOR] Generate (Complex) Record : ACTOR", BlueprintInternalUseOnly=true))
	FSaviorRecord GenerateRecord_Actor(const AActor* Actor);
	//
	//
	/** Unpacks (Complex) Object Data-Record from this Slot.
	@Object: Target Object for Code Reflection Analysis. */
	UFUNCTION(Category="Savior 2", meta=(DisplayName="[SAVIOR] Unpack (Complex) Record : OBJECT", BlueprintInternalUseOnly=true, ExpandEnumAsExecs="Result"))
	void UnpackRecord_Object(const FSaviorRecord &Record, UObject* Object, ESaviorResult &Result);
	//
	/** Unpacks (Complex) Component Data-Record from this Slot.
	@Component: Target Component for Code Reflection Analysis. */
	UFUNCTION(Category="Savior 2", meta=(DisplayName="[SAVIOR] Unpack (Complex) Record : COMPONENT", BlueprintInternalUseOnly=true, ExpandEnumAsExecs="Result"))
	void UnpackRecord_Component(const FSaviorRecord &Record, UActorComponent* Component, ESaviorResult &Result);
	//
	/** Unpacks (Complex) Actor Data-Record from this Slot.
	@Actor: Target Actor for Code Reflection Analysis. */
	UFUNCTION(Category="Savior 2", meta=(DisplayName="[SAVIOR] Unpack (Complex) Record : ACTOR", BlueprintInternalUseOnly=true, ExpandEnumAsExecs="Result"))
	void UnpackRecord_Actor(const FSaviorRecord &Record, AActor* Actor, ESaviorResult &Result);
	//
	//
	/** Generates (Minimal) Object Data-Record to be stored in this Slot.
	@Object: Target Object for Code Reflection Analysis. */
	UFUNCTION(Category="Savior 2", meta=(DisplayName="[SAVIOR] Generate (Minimal) Record : OBJECT", BlueprintInternalUseOnly=true))
	FSaviorMinimal GenerateMinimalRecord_Object(const UObject* Object);
	//
	/** Generates (Minimal) Component Data-Record to be stored in this Slot.
	@Component: Target Component for Code Reflection Analysis. */
	UFUNCTION(Category="Savior 2", meta=(DisplayName="[SAVIOR] Generate (Minimal) Record : COMPONENT", BlueprintInternalUseOnly=true))
	FSaviorMinimal GenerateMinimalRecord_Component(const UActorComponent* Component);
	//
	/** Generates (Minimal) Actor Data-Record to be stored in this Slot.
	@Actor: Target Actor for Code Reflection Analysis. */
	UFUNCTION(Category="Savior 2", meta=(DisplayName="[SAVIOR] Generate (Minimal) Record : ACTOR", BlueprintInternalUseOnly=true))
	FSaviorMinimal GenerateMinimalRecord_Actor(const AActor* Actor);
	//
	//
	/** Unpacks (Minimal) Object Data-Record from this Slot.
	@Object: Target Object for Code Reflection Analysis. */
	UFUNCTION(Category="Savior 2", meta=(DisplayName="[SAVIOR] Unpack (Minimal) Record : OBJECT", BlueprintInternalUseOnly=true, ExpandEnumAsExecs="Result"))
	void UnpackMinimalRecord_Object(const FSaviorMinimal &Record, UObject* Object, ESaviorResult &Result);
	//
	/** Unpacks (Minimal) Component Data-Record from this Slot.
	@Component: Target Component for Code Reflection Analysis. */
	UFUNCTION(Category="Savior 2", meta=(DisplayName="[SAVIOR] Unpack (Minimal) Record : COMPONENT", BlueprintInternalUseOnly=true, ExpandEnumAsExecs="Result"))
	void UnpackMinimalRecord_Component(const FSaviorMinimal &Record, UActorComponent* Component, ESaviorResult &Result);
	//
	/** Unpacks (Minimal) Actor Data-Record from this Slot.
	@Actor: Target Actor for Code Reflection Analysis. */
	UFUNCTION(Category="Savior 2", meta=(DisplayName="[SAVIOR] Unpack (Minimal) Record : ACTOR", BlueprintInternalUseOnly=true, ExpandEnumAsExecs="Result"))
	void UnpackMinimalRecord_Actor(const FSaviorMinimal &Record, AActor* Actor, ESaviorResult &Result);
	///
	/// :: UTILITY ::
	///
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Get Thread Safety"))
	static EThreadSafety GetThreadSafety() {return USavior2::ThreadSafety;}
	//
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Get Save Progress"))
	static float GetSaveProgress() {return USavior2::SS_Progress;}
	//
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Get Load Progress"))
	static float GetLoadProgress() {return USavior2::SL_Progress;}
	//
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Get Saves Done"))
	static float GetSavesDone() {return USavior2::SS_Complete;}
	//
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Get Loads Done"))
	static float GetLoadsDone() {return USavior2::SL_Complete;}
	//
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Calculate Save Workload"))
	const float GetSaveWorkload() {return (USavior2::SS_Workload=CalculateWorkload());}
	//
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Calculate Load Workload"))
	const float GetLoadWorkload() {return (USavior2::SL_Workload=CalculateWorkload());}
	//
	//
	/** Retrieves a copy of Slot's Data. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Get Slot Data"))
	TMap<FName,FSaviorRecord> GetSlotDataCopy();
	//
	/** Retrieves Slot's Thumbnail Image Asset, converted to 2D Texture, making it usable by UI Widgets. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Get Slot Thumbnail"))
	UTexture2D* GetSlotThumbnail(FVector2D &ImageSize);
	//
	/** Retrieves Slot's Save Time (Hour : Minutes) as Formatted String for UI/UX. */
	UFUNCTION(Category = "Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Get Save-Time ISO"))
	FString GetSaveTimeISO();
	//
	/** Retrieves Slot's Save Date as Formatted String for UI/UX. */
	UFUNCTION(Category = "Savior", BlueprintCallable, meta=(DisplayName="[SAVIOR] Get Save-Date ISO"))
	FString GetSaveDateISO();
	//
	//
	/** Destroy an Object and marks it to self-destroy on Level Load;
	Target Object must contain a Boolean Property, named 'Destroyed', which is accessed via Code Reflection in Runtime.
	@Object: Target Object to Auto-Destroy. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Mark Object (Auto-Destroy)", Keywords="Mark Destroy Object Savior"))
	static ESaviorResult MarkObjectAutoDestroyed(UObject* Object);
	//
	/** Destroy a Component and marks it to self-destroy on Level Load;
	Target Component must contain a Boolean Property, named 'Destroyed', which is accessed via Code Reflection in Runtime.
	@Component: Target Component to Auto-Destroy. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Mark Component (Auto-Destroy)", Keywords="Mark Destroy Component Savior"))
	static ESaviorResult MarkComponentAutoDestroyed(UActorComponent* Component);
	//
	/** Destroy an Actor and marks it to self-destroy on Level Load;
	Target Actor must contain a Boolean Property, named 'Destroyed', which is accessed via Code Reflection in Runtime.
	@Actor: Target Actor to Auto-Destroy. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Mark Actor (Auto-Destroy)", Keywords="Mark Destroy Actor Savior"))
	static ESaviorResult MarkActorAutoDestroyed(AActor* Actor);
	//
	/** Checks if an Object is Marked to be Auto-Destroyed. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Is Object Marked (Auto-Destroy)", Keywords="Mark Destroy Object Savior"))
	static bool IsObjectMarkedAutoDestroy(UObject* Object);
	//
	/** Checks if a Component is Marked to be Auto-Destroyed. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Is Component Marked (Auto-Destroy)", Keywords="Mark Destroy Component Savior"))
	static bool IsComponentMarkedAutoDestroy(UActorComponent* Component);
	//
	/** Checks if a Actor is Marked to be Auto-Destroyed. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Is Actor Marked (Auto-Destroy)", Keywords="Mark Destroy Actor Savior"))
	static bool IsActorMarkedAutoDestroy(AActor* Actor);
	//
	//
	/** Sets Value of 'Default Player ID' in Savior 2 Settings. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Set Default Player ID"))
	static void SetDefaultPlayerID(const int32 NewID);
	//
	/** Sets Value of 'Default Player Name' in Savior 2 Settings. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Set Default Player Name"))
	static void SetDefaultPlayerName(const FString NewName);
	//
	/** Sets Value of 'Default Player Level' in Savior 2 Settings. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Set Default Player Level"))
	static void SetDefaultPlayerLevel(const int32 NewLevel);
	//
	/** Sets Value of 'Default Chapter' in Savior 2 Settings. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Set Default Chapter"))
	static void SetDefaultChapter(const FString NewChapter);
	//
	/** Sets Value of 'Default Location' in Savior 2 Settings. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Set Default Save Location"))
	static void SetDefaultLocation(const FString NewLocation);
	//
	//
	/** Creates a new GUID only when Object's 'SGUID' is invalid.
	If existing SGUID is valid then no new values are generated for this Object.
	This is important for Runtime-Generated Objects that require a 'SGUID' to respawn from Save Data. */
	UFUNCTION(Category="Savior 2", BlueprintPure, meta=(DisplayName="[SAVIOR] Create Once : SGUID", WorldContext="Context"))
	static FGuid CreateSGUID(UObject* Context);
	//
	/** Checks if this Object's 'SGUID' (if any) matches compared Target Object's SGUID. */
	UFUNCTION(Category="Savior 2", BlueprintPure, meta=(DisplayName="[SAVIOR] Matches : SGUID", WorldContext="Context"))
	static bool MatchesGUID(UObject* Context, UObject* ComparedTo);
	//
	/** Look up current map (UWorld) for any Actor containing specified 'SGUID' property.
	Returns nothing if any Actor with GUID was not found in the level, check the returning value.
	@SGUID: Target Actor must contain a 'SGUID' Property of type FGuid marked 'SaveGame'. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Find Actor With GUID", WorldContext="Context"))
	static AActor* FindActorWithGUID(UObject* Context, const FGuid &SGUID);
	//
	//
	/** Creates a new Runtime Instance of a Savior Slot. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] New Slot Instance", WorldContext="Context", ExpandEnumAsExecs="Result"))
	static USavior2* NewSlotInstance(UObject* Context, USavior2* Slot, ESaviorResult &Result);
	//
	/** Loads the Default (Original, Root) Instance of an Object. */
	UFUNCTION(Category="Object", BlueprintCallable, meta = (DisplayName="Get Object of Class", Keywords = "Get Default Object"))
	static UObject* GetClassDefaultObject(UClass* Class);
	//
	/** Creates a Runtime New Instance of an Object. */
	UFUNCTION(Category="Object", BlueprintCallable, meta = (WorldContext = "Context", DisplayName="New Object Instance", Keywords = "New Object Instance"))
	static UObject* NewObjectInstance(UObject* Context, UClass* Class);
	//
	/** Creates a Runtime New Named Instance of an Object. */
	UFUNCTION(Category="Object", BlueprintCallable, meta = (WorldContext = "Context", DisplayName="New Object Instance", Keywords = "New Object Instance"))
	static UObject* NewNamedObjectInstance(UObject* Context, UClass* Class, FName Name);
	//
	//
	/** Generates a new valid GUID value. */
	UFUNCTION(Category="Object", BlueprintCallable, meta = (DisplayName="New Object GUID", Keywords = "New GUID"))
	static FGuid NewObjectGUID();
	//
	//
	/** Try to Load Object's Properties from Slot Data directly (if any).
	@Object: Object Reference loaded Properties are written to. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Object : (Slot Data)", WorldContext="Context", ExpandEnumAsExecs="Result"))
	static void StaticLoadObject(UObject* Object, const FSaviorRecord &Data, ESaviorResult &Result);
	//
	/** Try to Load Component's Properties from Slot Data directly (if any).
	@Component: Object Reference loaded Properties are written to. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Component : (Slot Data)", WorldContext="Context", ExpandEnumAsExecs="Result"))
	static void StaticLoadComponent(UActorComponent* Component, const FSaviorRecord &Data, ESaviorResult &Result);
	//
	/** Try to Load Actor's Properties from Slot Data directly (if any).
	@Actor: Object Reference loaded Properties are written to. */
	UFUNCTION(Category="Savior 2", BlueprintCallable, meta=(DisplayName="[SAVIOR] Load Actor : (Slot Data)", WorldContext="Context", ExpandEnumAsExecs="Result"))
	static void StaticLoadActor(AActor* Actor, const FSaviorRecord &Data, ESaviorResult &Result);
	//
	//
	UFUNCTION(Category="UI/UX", BlueprintCallable) float GetProgress();
	UFUNCTION(Category="UI/UX", BlueprintCallable) int32 GetPlayTime();
	UFUNCTION(Category="UI/UX", BlueprintCallable) FString GetChapter();
	UFUNCTION(Category="UI/UX", BlueprintCallable) int32 GetPlayerLevel();
	UFUNCTION(Category="UI/UX", BlueprintCallable) FDateTime GetSaveDate();
	UFUNCTION(Category="UI/UX", BlueprintCallable) FString GetPlayerName();
	UFUNCTION(Category="UI/UX", BlueprintCallable) FString GetSaveLocation();
	//
	UFUNCTION(Category="UI/UX", BlueprintCallable) void SetProgress(float New);
	UFUNCTION(Category="UI/UX", BlueprintCallable) void SetPlayTime(int32 New);
	UFUNCTION(Category="UI/UX", BlueprintCallable) void SetChapter(FString New);
	UFUNCTION(Category="UI/UX", BlueprintCallable) void SetPlayerLevel(int32 New);
	UFUNCTION(Category="UI/UX", BlueprintCallable) void SetSaveDate(FDateTime New);
	UFUNCTION(Category="UI/UX", BlueprintCallable) void SetPlayerName(FString New);
	UFUNCTION(Category="UI/UX", BlueprintCallable) void SetSaveLocation(FString New);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Savior Utilities:

/** Return True of an Actor hosts an specific Tag in its Tag List. */
SAVIOR2_API bool ParseActorTAG(const AActor* Actor, const FName Tag);

/** Return True of a Component hosts an specific Tag in its Tag List. */
SAVIOR2_API bool ParseComponentTAG(const UActorComponent* Component, const FName Tag);

static const FString SanitizePropertyID(const FString PID) {
	auto IX = PID.Find(TEXT("_"),ESearchCase::IgnoreCase,ESearchDir::FromEnd);
	return PID.Left(IX);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Savior Threading Utilities:

static void OnPreparedToSave() {
	for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
		if (!OBJ->IsValidLowLevelFast()) {continue;}
		if (OBJ->IsA(USavior2::StaticClass())) {continue;}
		if (OBJ->IsA(USaveGame::StaticClass())) {continue;}
		//
		const auto &Interface = Cast<ISAVIOR_Serializable>(*OBJ);
		//
		if (Interface) {Interface->Execute_OnPrepareToSave(*OBJ);}
		else if ((*OBJ)->GetClass()->ImplementsInterface(USAVIOR_Serializable::StaticClass())) {
			ISAVIOR_Serializable::Execute_OnPrepareToSave(*OBJ);
		}///
	}///
}

static void OnPreparedToLoad() {
	for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
		if (!OBJ->IsValidLowLevelFast()) {continue;}
		if (OBJ->IsA(USavior2::StaticClass())) {continue;}
		if (OBJ->IsA(USaveGame::StaticClass())) {continue;}
		//
		const auto &Interface = Cast<ISAVIOR_Serializable>(*OBJ);
		//
		if (Interface) {Interface->Execute_OnPrepareToLoad(*OBJ);}
		else if ((*OBJ)->GetClass()->ImplementsInterface(USAVIOR_Serializable::StaticClass())) {
			ISAVIOR_Serializable::Execute_OnPrepareToLoad(*OBJ);
		}///
	}///
}

static void OnObjectSaved(UObject* Object) {
	const auto &Interface = Cast<ISAVIOR_Serializable>(Object);
	//
	if (Interface) {Interface->Execute_OnSaved(Object);}
	else if (Object->GetClass()->ImplementsInterface(USAVIOR_Serializable::StaticClass())) {
		ISAVIOR_Serializable::Execute_OnSaved(Object);
	}///
}

static void OnActorSaved(AActor* Actor) {
	const auto &Interface = Cast<ISAVIOR_Serializable>(Actor);
	//
	if (Interface) {Interface->Execute_OnSaved(Actor);}
	else if (Actor->GetClass()->ImplementsInterface(USAVIOR_Serializable::StaticClass())) {
		ISAVIOR_Serializable::Execute_OnSaved(Actor);
	}///
}

static void OnComponentSaved(UActorComponent* Component) {
	const auto &Interface = Cast<ISAVIOR_Serializable>(Component);
	//
	if (Interface) {Interface->Execute_OnSaved(Component);}
	else if (Component->GetClass()->ImplementsInterface(USAVIOR_Serializable::StaticClass())) {
		ISAVIOR_Serializable::Execute_OnSaved(Component);
	}///
}

static void OnObjectLoaded(const FSlotMetaData Meta, UObject* Object) {
	const auto &Interface = Cast<ISAVIOR_Serializable>(Object);
	//
	if (Interface) {Interface->Execute_OnLoaded(Object,Meta);}
	else if (Object->GetClass()->ImplementsInterface(USAVIOR_Serializable::StaticClass())) {
		ISAVIOR_Serializable::Execute_OnLoaded(Object,Meta);
	}///
}

static void OnActorLoaded(const FSlotMetaData Meta, AActor* Actor) {
	const auto &Interface = Cast<ISAVIOR_Serializable>(Actor);
	//
	if (Interface) {Interface->Execute_OnLoaded(Actor,Meta);}
	else if (Actor->GetClass()->ImplementsInterface(USAVIOR_Serializable::StaticClass())) {
		ISAVIOR_Serializable::Execute_OnLoaded(Actor,Meta);
	}///
}

static void OnComponentLoaded(const FSlotMetaData Meta, UActorComponent* Component) {
	const auto &Interface = Cast<ISAVIOR_Serializable>(Component);
	//
	if (Interface) {Interface->Execute_OnLoaded(Component,Meta);}
	else if (Component->GetClass()->ImplementsInterface(USAVIOR_Serializable::StaticClass())) {
		ISAVIOR_Serializable::Execute_OnLoaded(Component,Meta);
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Savior Threading Interface [Game World]:

static void OnFinishSerializeGameWorld(USavior2* Savior, ESaviorResult Result) {
	check(IsInGameThread());
	//
	const auto &Settings = GetDefault<USavior2Settings>();
	Savior->WriteSlotToFile(Settings->DefaultPlayerID,Result);
	//
	Savior->EVENT_OnFinishDataSAVE.Broadcast(Result==ESaviorResult::Success);
	Savior->ClearWorkload();
}

///	(C) 2018 - Bruno Xavier B. Leite
static void SerializeGameWorld(USavior2* Savior) {
	if (Savior==nullptr||!Savior->IsValidLowLevelFast()||Savior->GetWorld()==nullptr) {return;}
	const auto &Settings = GetDefault<USavior2Settings>();
	ESaviorResult Result;
	//
	//
	for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
		if (!OBJ->IsValidLowLevelFast()) {continue;}
		if (OBJ->IsA(USavior2::StaticClass())) {continue;}
		if (OBJ->IsA(USaveGame::StaticClass())) {continue;}
		//
		if (OBJ->IsA(UGameInstance::StaticClass())) {
			Savior->SaveGameInstanceSingleTon(CastChecked<UGameInstance>(*OBJ),Result);
		continue;}
		//
		if ((*OBJ)==Savior->GetWorld()) {continue;}
		if (!OBJ->GetOutermost()->ContainsMap()) {continue;}
		if (OBJ->GetWorld()!=Savior->GetWorld()) {continue;}
		if (OBJ->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject|RF_BeginDestroyed)) {continue;}
		//
		bool IsTarget = false;
		for (auto ACT : Savior->ActorScope) { if (ACT.Get()&&(OBJ->IsA(ACT))) {IsTarget=true; break;} }
		for (auto OBS : Savior->ObjectScope) { if (OBS.Get()&&(OBJ->IsA(OBS))) {IsTarget=true; break;} }
		for (auto COM : Savior->ComponentScope) { if (COM.Get()&&(OBJ->IsA(COM))) {IsTarget=true; break;} }
		//
		if (IsTarget) {
			if (OBJ->IsA(UActorComponent::StaticClass())) {Savior->SaveComponent(Savior->GetWorld(),CastChecked<UActorComponent>(*OBJ),Result);} else
			if (OBJ->IsA(AActor::StaticClass())) {Savior->SaveActor(Savior->GetWorld(),CastChecked<AActor>(*OBJ),Result);}
			else {Savior->SaveObjectHierarchy((*OBJ),Result);}
		}///
	}///
	//
	//
	Result = ESaviorResult::Success;
	if (IsInGameThread()) {OnFinishSerializeGameWorld(Savior,Result);} else {
		FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
			FSimpleDelegateGraphTask::FDelegate::CreateStatic(&OnFinishSerializeGameWorld,Savior,Result),
			GET_STATID(STAT_FSimpleDelegateGraphTask_SerializeGameWorld),
			nullptr, ENamedThreads::GameThread
		);//
	}///
}

static void OnFinishDeserializeGameWorld(USavior2* Savior, ESaviorResult Result) {
	check(IsInGameThread());
	const auto &Settings = GetDefault<USavior2Settings>();
	//
	Savior->EVENT_OnFinishDataLOAD.Broadcast(Result==ESaviorResult::Success);
	Savior->ClearWorkload();
	//
	const auto &PlayerController = UGameplayStatics::GetPlayerController(Savior,Settings->DefaultPlayerID);
	const auto &Pawn = UGameplayStatics::GetPlayerPawn(Savior,Settings->DefaultPlayerID);
	//
	if ((PlayerController!=nullptr)&&(Pawn!=nullptr)) {
		if (PlayerController->GetRootComponent()) {PlayerController->GetRootComponent()->SetUsingAbsoluteRotation(true);}
		PlayerController->SetControlRotation(Pawn->ActorToWorld().GetRotation().Rotator());
		if (PlayerController->GetRootComponent()) {PlayerController->GetRootComponent()->SetUsingAbsoluteRotation(false);}
	}///
}

///	(C) 2018 - Bruno Xavier B. Leite
static void DeserializeGameWorld(USavior2* Savior) {
	if (Savior==nullptr||Savior->GetWorld()==nullptr) {return;}
	const auto &Settings = GetDefault<USavior2Settings>();
	ESaviorResult Result;
	//
	//
	TMap<FName,FSaviorRecord>SlotData = Savior->GetSlotDataCopy();
	TMap<FName,FSaviorRecord>DES_Data = SlotData;
	TMap<FName,FSaviorRecord>ACT_Data;
	TMap<FName,FSaviorRecord>CMP_Data;
	//
	if (Savior->Compression==ERecordType::Complex) {
		for (auto Pair : DES_Data) {
			if (Pair.Value.Destroyed) {SlotData.Remove(Pair.Key); continue;}
		} DES_Data.Empty();
	}///
	//
	if (Settings->RespawnDynamicActors && Savior->Compression==ERecordType::Complex) {
		for (auto Pair : SlotData) {
			if (FPackageName::IsShortPackageName(Pair.Value.FullName)) {continue;}
			//
			const FSoftObjectPath ObjectPath(Pair.Value.FullName);
			const auto &OBJ = ObjectPath.ResolveObject();
			if (OBJ!=nullptr) {continue;}
			//
			const FSoftObjectPath ClassPath(Pair.Value.ClassPath);
			const auto CDO = ClassPath.ResolveObject();
			//
			if ((CDO!=nullptr)&&(CDO->IsValidLowLevelFast())&&(CDO->IsA(AActor::StaticClass()))) {
				const auto &IT = Cast<ISAVIOR_Procedural>(CDO);
				if (IT||CDO->GetClass()->ImplementsInterface(USAVIOR_Procedural::StaticClass())) {
					ACT_Data.Emplace(Pair.Key,Pair.Value);
				}///
			}///
		}///
		//
		for (auto Pair : ACT_Data) {
			if (IsInGameThread()) {Reflector::RespawnActorFromData(Savior->GetWorld(),Settings->RespawnScope,Pair.Value);} else {
				FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
					FSimpleDelegateGraphTask::FDelegate::CreateStatic(&Reflector::RespawnActorFromData,Savior->GetWorld(),Settings->RespawnScope,Pair.Value),
					GET_STATID(STAT_FSimpleDelegateGraphTask_RespawnActorFromData),
					nullptr, ENamedThreads::GameThread
				);//
		}}//////
	}///
	//
	if (Settings->RespawnDynamicComponents && Savior->Compression==ERecordType::Complex) {
		for (auto Pair : SlotData) {
			if (FPackageName::IsShortPackageName(Pair.Value.FullName)) {continue;}
			//
			const FSoftObjectPath ObjectPath(Pair.Value.FullName);
			const auto &OBJ = ObjectPath.ResolveObject();
			if (OBJ!=nullptr) {continue;}
			//
			const FSoftObjectPath ClassPath(Pair.Value.ClassPath);
			const auto CDO = ClassPath.ResolveObject();
			//
			if ((CDO!=nullptr)&&(CDO->IsA(UActorComponent::StaticClass()))) {
				const auto &IT = Cast<ISAVIOR_Procedural>(CDO);
				if (IT||CDO->GetClass()->ImplementsInterface(USAVIOR_Procedural::StaticClass())) {
					CMP_Data.Emplace(Pair.Key,Pair.Value);
				}///
			}///
		}///
		//
		for (auto Pair : CMP_Data) {
			if (IsInGameThread()) {Reflector::RespawnComponentFromData(Savior->GetWorld(),Settings->RespawnScope,Pair.Value);} else {
				FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
					FSimpleDelegateGraphTask::FDelegate::CreateStatic(&Reflector::RespawnComponentFromData,Savior->GetWorld(),Settings->RespawnScope,Pair.Value),
					GET_STATID(STAT_FSimpleDelegateGraphTask_RespawnComponentFromData),
					nullptr, ENamedThreads::GameThread
				);//
		}}//////
	}///
	//
	//
	for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
		if (!OBJ->IsValidLowLevelFast()) {continue;}
		if (OBJ->IsA(USavior2::StaticClass())) {continue;}
		if (OBJ->IsA(USaveGame::StaticClass())) {continue;}
		//
		if (OBJ->IsA(UGameInstance::StaticClass())) {
			Savior->LoadGameInstanceSingleTon(CastChecked<UGameInstance>(*OBJ),Result);
		continue;}
		//
		if ((*OBJ)==Savior->GetWorld()) {continue;}
		if (!OBJ->GetOutermost()->ContainsMap()) {continue;}
		if (OBJ->GetWorld()!=Savior->GetWorld()) {continue;}
		if (OBJ->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject|RF_BeginDestroyed)) {continue;}
		//
		bool IsTarget = false;
		for (auto ACT : Savior->ActorScope) { if (ACT.Get()&&(OBJ->IsA(ACT))) {IsTarget=true; break;} }
		for (auto OBS : Savior->ObjectScope) { if (OBS.Get()&&(OBJ->IsA(OBS))) {IsTarget=true; break;} }
		for (auto COM : Savior->ComponentScope) { if (COM.Get()&&(OBJ->IsA(COM))) {IsTarget=true; break;} }
		//
		if (IsTarget) {
			if (OBJ->IsA(UActorComponent::StaticClass())) {Savior->LoadComponent(Savior->GetWorld(),CastChecked<UActorComponent>(*OBJ),Result);} else
			if (OBJ->IsA(AActor::StaticClass())) {
				Savior->LoadActor(Savior->GetWorld(),CastChecked<AActor>(*OBJ),Result);
			} else {Savior->LoadObjectHierarchy((*OBJ),Result);}
		}///
	}///
	//
	//
	Result = ESaviorResult::Success;
	if (IsInGameThread()) {OnFinishDeserializeGameWorld(Savior,Result);} else {
		FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
			FSimpleDelegateGraphTask::FDelegate::CreateStatic(&OnFinishDeserializeGameWorld,Savior,Result),
			GET_STATID(STAT_FSimpleDelegateGraphTask_DeserializeGameWorld),
			nullptr, ENamedThreads::GameThread
		);//
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Property Threading Interface:

static void GT_NewObjectInstance(USavior2* Savior, FObjectProperty* Property, UObject* Outer, UObject* CDO, const FString FullName) {
	if (!IsInGameThread()||(Outer==nullptr)||(CDO==nullptr)||(Property==nullptr)) {return;}
	//
	auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Outer);
	FString SGUID; FGuid GUID; GUID.Invalidate();
	ESaviorResult Result;
	//
	if (FullName.Split(TEXT("_"),nullptr,&SGUID,ESearchCase::IgnoreCase,ESearchDir::FromEnd)) {
		FGuid::Parse(SGUID,GUID);
		//
		if (GUID.IsValid()) {
			for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
				if (!OBJ->GetOutermost()->ContainsMap()) {continue;}
				if (OBJ->HasAnyFlags(RF_BeginDestroyed)) {continue;}
				if (OBJ->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {continue;}
				//
				if (Reflector::FindGUID(*OBJ)==GUID) {
					Savior->LoadObject(*OBJ,Result);
					Property->SetPropertyValue(ValuePtr,(*OBJ));
				return;}
			}///
		}///
	} else {
		for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
			if (!OBJ->GetOutermost()->ContainsMap()) {continue;}
			if (OBJ->HasAnyFlags(RF_BeginDestroyed)) {continue;}
			if (OBJ->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {continue;}
			//
			if ((*OBJ)->IsA(AActor::StaticClass())) {
				if (Reflector::MakeActorID(CastChecked<AActor>(*OBJ),true).ToString()==FullName){Property->SetPropertyValue(ValuePtr,*OBJ); return;}
			} else if ((*OBJ)->IsA(UActorComponent::StaticClass())) {
				if (Reflector::MakeComponentID(CastChecked<UActorComponent>(*OBJ),true).ToString()==FullName) {Property->SetPropertyValue(ValuePtr,*OBJ); return;}
			} else {
				if (Reflector::MakeObjectID(*OBJ).ToString()==FullName) {
					Savior->LoadObject(*OBJ,Result);
					Property->SetPropertyValue(ValuePtr,*OBJ);
				return;}
			}///
		}///
	}///
	//
	//
	FString Name = (SGUID.IsEmpty()) ? CDO->GetName() : (CDO->GetName()+TEXT("_0")).Replace(TEXT("Default__"),TEXT(""));
	auto OBJ = NewObject<UObject>(Outer,CDO->GetClass(),*Name,RF_NoFlags,CDO);
	Reflector::SetGUID(GUID,OBJ);
	//
	if (Savior) {Savior->LoadObject(OBJ,Result);}
	//
	Property->SetPropertyValue(ValuePtr,OBJ);
}

static void GT_NewObjectIntoArray(USavior2* Savior, FArrayProperty* Property, UObject* Outer, UObject* CDO, const FString FullName, const int32 Index) {
	if (!IsInGameThread()||(Outer==nullptr)||(CDO==nullptr)||(Property==nullptr)) {return;}
	//
	TArray<UObject*>&Array = *Property->ContainerPtrToValuePtr<TArray<UObject*>>(Outer);
	if (!Array.IsValidIndex(Index)) {return;}
	//
	ESaviorResult Result;
	FString SGUID; FGuid GUID; GUID.Invalidate();
	if (FullName.Split(TEXT("_"),nullptr,&SGUID,ESearchCase::IgnoreCase,ESearchDir::FromEnd)) {
		FGuid::Parse(SGUID,GUID);
		//
		if (GUID.IsValid()) {
			for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
				if (!OBJ->GetOutermost()->ContainsMap()) {continue;}
				if (OBJ->HasAnyFlags(RF_BeginDestroyed)) {continue;}
				if (OBJ->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {continue;}
				//
				if (Reflector::FindGUID(*OBJ)==GUID) {
					Savior->LoadObject(*OBJ,Result);
					Array[Index]=(*OBJ);
				return;}
			}///
		}///
	} else {
		for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
			if (!OBJ->GetOutermost()->ContainsMap()) {continue;}
			if (OBJ->HasAnyFlags(RF_BeginDestroyed)) {continue;}
			if (OBJ->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {continue;}
			//
			if ((*OBJ)->IsA(AActor::StaticClass())) {
				if (Reflector::MakeActorID(CastChecked<AActor>(*OBJ),true).ToString()==FullName){Array[Index]=(*OBJ); return;}
			} else if ((*OBJ)->IsA(UActorComponent::StaticClass())) {
				if (Reflector::MakeComponentID(CastChecked<UActorComponent>(*OBJ),true).ToString()==FullName) {Array[Index]=(*OBJ); return;}
			} else {
				if (Reflector::MakeObjectID(*OBJ).ToString()==FullName) {
					Savior->LoadObject(*OBJ,Result);
					Array[Index]=(*OBJ);
				return;}
			}///
		}///
	}///
	//
	//
	FString Name = (SGUID.IsEmpty()) ? CDO->GetName() : (CDO->GetName()+TEXT("_")+FString::FromInt(Index)).Replace(TEXT("Default__"),TEXT(""));
	auto OBJ = NewObject<UObject>(Outer,CDO->GetClass(),*Name,RF_NoFlags,CDO);
	Reflector::SetGUID(GUID,OBJ);
	//
	if (Savior) {Savior->LoadObject(OBJ,Result);}
	//
	Array[Index] = (OBJ);
}

static void GT_NewObjectIntoSet(USavior2* Savior, FSetProperty* Property, UObject* Outer, UObject* CDO, const FString FullName) {
	if (!IsInGameThread()||(Outer==nullptr)||(CDO==nullptr)||(Property==nullptr)) {return;}
	//
	TSet<UObject*>&Set = *Property->ContainerPtrToValuePtr<TSet<UObject*>>(Outer);
	//
	ESaviorResult Result;
	FString SGUID; FGuid GUID; GUID.Invalidate();
	if (FullName.Split(TEXT("_"),nullptr,&SGUID,ESearchCase::IgnoreCase,ESearchDir::FromEnd)) {
		FGuid::Parse(SGUID,GUID);
		//
		if (GUID.IsValid()) {
			for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
				if (!OBJ->GetOutermost()->ContainsMap()) {continue;}
				if (OBJ->HasAnyFlags(RF_BeginDestroyed)) {continue;}
				if (OBJ->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {continue;}
				//
				if (Reflector::FindGUID(*OBJ)==GUID) {
					Savior->LoadObject(*OBJ,Result);
					Set.Add(*OBJ);
				return;}
			}///
		}///
	} else {
		for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
			if (!OBJ->GetOutermost()->ContainsMap()) {continue;}
			if (OBJ->HasAnyFlags(RF_BeginDestroyed)) {continue;}
			if (OBJ->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {continue;}
			//
			if ((*OBJ)->IsA(AActor::StaticClass())) {
				if (Reflector::MakeActorID(CastChecked<AActor>(*OBJ),true).ToString()==FullName){Set.Add(*OBJ); return;}
			} else if ((*OBJ)->IsA(UActorComponent::StaticClass())) {
				if (Reflector::MakeComponentID(CastChecked<UActorComponent>(*OBJ),true).ToString()==FullName) {Set.Add(*OBJ); return;}
			} else {
				if (Reflector::MakeObjectID(*OBJ).ToString()==FullName) {
					Savior->LoadObject(*OBJ,Result); Set.Add(*OBJ);
				return;}
			}///
		}///
	}///
	//
	//
	FString Name = (SGUID.IsEmpty()) ? CDO->GetName() : CDO->GetName().Replace(TEXT("Default__"),TEXT(""));
	auto OBJ = NewObject<UObject>(Outer,CDO->GetClass(),*Name,RF_NoFlags,CDO);
	Reflector::SetGUID(GUID,OBJ);
	//
	if (Savior) {Savior->LoadObject(OBJ,Result);}
	//
	Set.Add(OBJ);
}

static void GT_NewObjectIntoMapKey(USavior2* Savior, FMapProperty* Property, UObject* Outer, UObject* CDO, const FString FullName) {
	if (!IsInGameThread()||(Outer==nullptr)||(CDO==nullptr)||(Property==nullptr)) {return;}
	//
	TMap<UObject*,void*>&Map = *Property->ContainerPtrToValuePtr<TMap<UObject*,void*>>(Outer);
	FString SGUID; FGuid GUID; GUID.Invalidate();
	//
	if (FullName.Split(TEXT("_"),nullptr,&SGUID,ESearchCase::IgnoreCase,ESearchDir::FromEnd)) {
		FGuid::Parse(SGUID,GUID);
		//
		if (GUID.IsValid()) {
			for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
				if (!OBJ->GetOutermost()->ContainsMap()) {continue;}
				if (OBJ->HasAnyFlags(RF_BeginDestroyed)) {continue;}
				if (OBJ->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {continue;}
				//
				if (Reflector::FindGUID(*OBJ)==GUID) {
					Map.Add(*OBJ); return;
				}///
			}///
		}///
	}///
	//
	FString Name = (SGUID.IsEmpty()) ? CDO->GetName() : CDO->GetName().Replace(TEXT("Default__"),TEXT(""));
	auto OBJ = NewObject<UObject>(Outer,CDO->GetClass(),*Name,RF_NoFlags,CDO);
	//
	Reflector::SetGUID(GUID,OBJ);
	//
	ESaviorResult Result;
	if (Savior){Savior->LoadObject(OBJ,Result);}
	//
	Map.Add(OBJ);
}

static void GT_NewObjectIntoMapValue(USavior2* Savior, FMapProperty* Property, UObject* Outer, UObject* Key, UObject* CDO, const FString FullName) {
	if (!IsInGameThread()||(Outer==nullptr)||(CDO==nullptr)||(Property==nullptr)) {return;}
	//
	TMap<UObject*,UObject*>&Map = *Property->ContainerPtrToValuePtr<TMap<UObject*,UObject*>>(Outer);
	FString SGUID; FGuid GUID; GUID.Invalidate();
	//
	if (FullName.Split(TEXT("_"),nullptr,&SGUID,ESearchCase::IgnoreCase,ESearchDir::FromEnd)) {
		FGuid::Parse(SGUID,GUID);
		//
		if (GUID.IsValid()) {
			for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
				if (!OBJ->GetOutermost()->ContainsMap()) {continue;}
				if (OBJ->HasAnyFlags(RF_BeginDestroyed)) {continue;}
				if (OBJ->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {continue;}
				//
				if (Reflector::FindGUID(*OBJ)==GUID) {
					Map.Emplace(Key,*OBJ); return;
				}///
			}///
		}///
	}///
	//
	FString Name = (SGUID.IsEmpty()) ? CDO->GetName() : CDO->GetName().Replace(TEXT("Default__"),TEXT(""));
	auto OBJ = NewObject<UObject>(Outer,CDO->GetClass(),*Name,RF_NoFlags,CDO);
	//
	Reflector::SetGUID(GUID,OBJ);
	//
	ESaviorResult Result;
	if (Savior){Savior->LoadObject(OBJ,Result);}
	//
	Map.Emplace(Key,OBJ);
}

static void GT_NewObjectIntoNamedMapValue(USavior2* Savior, FMapProperty* Property, UObject* Outer, FName Key, UObject* CDO, const FString FullName) {
	if (!IsInGameThread()||(Outer==nullptr)||(CDO==nullptr)||(Property==nullptr)) {return;}
	//
	TMap<FName,UObject*>&Map = *Property->ContainerPtrToValuePtr<TMap<FName,UObject*>>(Outer);
	FString SGUID; FGuid GUID; GUID.Invalidate();
	//
	if (FullName.Split(TEXT("_"),nullptr,&SGUID,ESearchCase::IgnoreCase,ESearchDir::FromEnd)) {
		FGuid::Parse(SGUID,GUID);
		//
		if (GUID.IsValid()) {
			for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
				if (!OBJ->GetOutermost()->ContainsMap()) {continue;}
				if (OBJ->HasAnyFlags(RF_BeginDestroyed)) {continue;}
				if (OBJ->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {continue;}
				//
				if (Reflector::FindGUID(*OBJ)==GUID) {
					Map.Emplace(Key,*OBJ); return;
				}///
			}///
		}///
	}///
	//
	FString Name = (SGUID.IsEmpty()) ? CDO->GetName() : CDO->GetName().Replace(TEXT("Default__"),TEXT(""));
	auto OBJ = NewObject<UObject>(Outer,CDO->GetClass(),*Name,RF_NoFlags,CDO);
	//
	Reflector::SetGUID(GUID,OBJ);
	//
	ESaviorResult Result;
	if (Savior){Savior->LoadObject(OBJ,Result);}
	//
	Map.Emplace(Key,OBJ);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Savior Threading Interface [Level]:

static void OnFinishSerializeLevel(USavior2* Savior, ESaviorResult Result) {
	check(IsInGameThread());
	//
	const auto &Settings = GetDefault<USavior2Settings>();
	Savior->WriteSlotToFile(Settings->DefaultPlayerID,Result);
	//
	Savior->EVENT_OnFinishDataSAVE.Broadcast(Result==ESaviorResult::Success);
	Savior->ClearWorkload();
}

///	(C) 2018 - Bruno Xavier B. Leite
static void SerializeLevel(USavior2* Savior, const FName LevelToSave) {
	if (Savior==nullptr||Savior->GetWorld()==nullptr||LevelToSave.IsNone()) {return;}
	const auto &Settings = GetDefault<USavior2Settings>();
	ESaviorResult Result;
	//
	//
	for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
		if (!OBJ->IsValidLowLevelFast()) {continue;}
		if (OBJ->IsA(USavior2::StaticClass())) {continue;}
		if (OBJ->IsA(USaveGame::StaticClass())) {continue;}
		//
		if ((*OBJ)==Savior->GetWorld()) {continue;}
		if (!OBJ->GetOutermost()->ContainsMap()) {continue;}
		if (OBJ->GetWorld()!=Savior->GetWorld()) {continue;}
		if (OBJ->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject|RF_BeginDestroyed)) {continue;}
		if (!OBJ->GetOutermost()->GetName().Contains(LevelToSave.ToString(),ESearchCase::IgnoreCase)) {continue;}
		//
		bool IsTarget = false;
		for (auto ACT : Savior->ActorScope) { if (ACT.Get()&&(OBJ->IsA(ACT))) {IsTarget=true; break;} }
		for (auto OBS : Savior->ObjectScope) { if (OBS.Get()&&(OBJ->IsA(OBS))) {IsTarget=true; break;} }
		for (auto COM : Savior->ComponentScope) { if (COM.Get()&&(OBJ->IsA(COM))) {IsTarget=true; break;} }
		//
		if (IsTarget) {
			if (OBJ->IsA(UActorComponent::StaticClass())) {Savior->SaveComponent(Savior->GetWorld(),CastChecked<UActorComponent>(*OBJ),Result);} else
			if (OBJ->IsA(AActor::StaticClass())) {Savior->SaveActor(Savior->GetWorld(),CastChecked<AActor>(*OBJ),Result);}
			else {Savior->SaveObject((*OBJ),Result);}
		}///
	}///
	//
	//
	Result = ESaviorResult::Success;
	if (IsInGameThread()) {OnFinishSerializeLevel(Savior,Result);} else {
		FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
			FSimpleDelegateGraphTask::FDelegate::CreateStatic(&OnFinishSerializeLevel,Savior,Result),
			GET_STATID(STAT_FSimpleDelegateGraphTask_SerializeLevel),
			nullptr, ENamedThreads::GameThread
		);//
	}///
}

static void OnFinishDeserializeLevel(USavior2* Savior, ESaviorResult Result) {
	check(IsInGameThread());
	//
	Savior->EVENT_OnFinishDataLOAD.Broadcast(Result==ESaviorResult::Success);
	Savior->ClearWorkload();
}

///	(C) 2018 - Bruno Xavier B. Leite
static void DeserializeLevel(USavior2* Savior, const FName LevelToLoad) {
	if (Savior==nullptr||Savior->GetWorld()==nullptr||LevelToLoad.IsNone()) {return;}
	const auto &Settings = GetDefault<USavior2Settings>();
	ESaviorResult Result;
	//
	//
	for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
		if (!OBJ->IsValidLowLevelFast()) {continue;}
		if (OBJ->IsA(USavior2::StaticClass())) {continue;}
		if (OBJ->IsA(USaveGame::StaticClass())) {continue;}
		//
		if ((*OBJ)==Savior->GetWorld()) {continue;}
		if (!OBJ->GetOutermost()->ContainsMap()) {continue;}
		if (OBJ->GetWorld()!=Savior->GetWorld()) {continue;}
		if (OBJ->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject|RF_BeginDestroyed)) {continue;}
		if (!OBJ->GetOutermost()->GetName().Contains(LevelToLoad.ToString(),ESearchCase::IgnoreCase)) {continue;}
		//
		bool IsTarget = false;
		for (auto ACT : Savior->ActorScope) { if (ACT.Get()&&(OBJ->IsA(ACT))) {IsTarget=true; break;} }
		for (auto OBS : Savior->ObjectScope) { if (OBS.Get()&&(OBJ->IsA(OBS))) {IsTarget=true; break;} }
		for (auto COM : Savior->ComponentScope) { if (COM.Get()&&(OBJ->IsA(COM))) {IsTarget=true; break;} }
		//
		if (IsTarget) {
			if (OBJ->IsA(UActorComponent::StaticClass())) {Savior->LoadComponent(Savior->GetWorld(),CastChecked<UActorComponent>(*OBJ),Result);} else
			if (OBJ->IsA(AActor::StaticClass())) {Savior->LoadActor(Savior->GetWorld(),CastChecked<AActor>(*OBJ),Result);}
			else {Savior->LoadObject((*OBJ),Result);}
		}///
	}///
	//
	//
	Result = ESaviorResult::Success;
	if (IsInGameThread()) {OnFinishDeserializeLevel(Savior,Result);} else {
		FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
			FSimpleDelegateGraphTask::FDelegate::CreateStatic(&OnFinishDeserializeLevel,Savior,Result),
			GET_STATID(STAT_FSimpleDelegateGraphTask_DeserializeLevel),
			nullptr, ENamedThreads::GameThread
		);//
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Savior Threading Interface [Game-Mode]:

static void OnFinishSerializeGameMode(USavior2* Savior, ESaviorResult Result) {
	check(IsInGameThread());
	//
	const auto &Settings = GetDefault<USavior2Settings>();
	Savior->WriteSlotToFile(Settings->DefaultPlayerID,Result);
	//
	Savior->EVENT_OnFinishDataSAVE.Broadcast(Result==ESaviorResult::Success);
	Savior->ClearWorkload();
}

///	(C) 2018 - Bruno Xavier B. Leite
static void SerializeGameMode(USavior2* Savior) {
	if (Savior==nullptr||Savior->GetWorld()==nullptr) {return;}
	const auto &Settings = GetDefault<USavior2Settings>();
	ESaviorResult Result;
	//
	//
	const auto &GameMode = Savior->GetWorld()->GetAuthGameMode();
	if ((GameMode!=nullptr)&&GameMode->IsValidLowLevelFast()) {
		Savior->SaveActor(Savior->GetWorld(),GameMode,Result);
	}///
	//
	const auto &GameState = Savior->GetWorld()->GetGameState();
	if ((GameState!=nullptr)&&GameState->IsValidLowLevelFast()) {
		Savior->SaveActor(Savior->GetWorld(),GameState,Result);
	}///
	//
	const auto &PlayerController = UGameplayStatics::GetPlayerController(Savior,Settings->DefaultPlayerID);
	if ((PlayerController!=nullptr)&&PlayerController->IsValidLowLevelFast()) {
		const auto &Pawn = PlayerController->GetPawn();
		const auto &PlayerHUD = PlayerController->MyHUD;
		const auto &PlayerState = PlayerController->PlayerState;
		//
		Savior->SaveActor(Savior->GetWorld(),PlayerController,Result);
		//
		if ((Pawn!=nullptr)&&Pawn->IsValidLowLevelFast()) {Savior->SaveActor(Savior->GetWorld(),Pawn,Result);}
		if ((PlayerHUD!=nullptr)&&PlayerHUD->IsValidLowLevelFast()) {Savior->SaveActor(Savior->GetWorld(),PlayerHUD,Result);}
		if ((PlayerState!=nullptr)&&PlayerState->IsValidLowLevelFast()) {Savior->SaveActor(Savior->GetWorld(),PlayerState,Result);}
	}///
	//
	//
	Result = ESaviorResult::Success;
	if (IsInGameThread()) {OnFinishSerializeGameMode(Savior,Result);} else {
		FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
			FSimpleDelegateGraphTask::FDelegate::CreateStatic(&OnFinishSerializeGameMode,Savior,Result),
			GET_STATID(STAT_FSimpleDelegateGraphTask_SerializeGameMode),
			nullptr, ENamedThreads::GameThread
		);//
	}///
}

static void OnFinishDeserializeGameMode(USavior2* Savior, ESaviorResult Result) {
	check(IsInGameThread());
	const auto &Settings = GetDefault<USavior2Settings>();
	//
	Savior->EVENT_OnFinishDataLOAD.Broadcast(Result==ESaviorResult::Success);
	Savior->ClearWorkload();
	//
	const auto &PlayerController = UGameplayStatics::GetPlayerController(Savior,Settings->DefaultPlayerID);
	const auto &Pawn = UGameplayStatics::GetPlayerPawn(Savior,Settings->DefaultPlayerID);
	//
	if ((PlayerController!=nullptr)&&(Pawn!=nullptr)) {
		if (PlayerController->GetRootComponent()) {PlayerController->GetRootComponent()->SetUsingAbsoluteRotation(true);}
		PlayerController->SetControlRotation(Pawn->ActorToWorld().GetRotation().Rotator());
		if (PlayerController->GetRootComponent()) {PlayerController->GetRootComponent()->SetUsingAbsoluteRotation(false);}
	}///
}

///	(C) 2018 - Bruno Xavier B. Leite
static void DeserializeGameMode(USavior2* Savior) {
	if (Savior==nullptr||Savior->GetWorld()==nullptr) {return;}
	const auto &Settings = GetDefault<USavior2Settings>();
	ESaviorResult Result;
	//
	//
	const auto &GameMode = Savior->GetWorld()->GetAuthGameMode();
	if ((GameMode!=nullptr)&&GameMode->IsValidLowLevelFast()) {
		Savior->LoadActor(Savior->GetWorld(),GameMode,Result);
	}///
	//
	const auto &GameState = Savior->GetWorld()->GetGameState();
	if ((GameState!=nullptr)&&GameState->IsValidLowLevelFast()) {
		Savior->LoadActor(Savior->GetWorld(),GameState,Result);
	}///
	//
	const auto &PlayerController = UGameplayStatics::GetPlayerController(Savior,Settings->DefaultPlayerID);
	if ((PlayerController!=nullptr)&&PlayerController->IsValidLowLevelFast()) {
		const auto &Pawn = PlayerController->GetPawn();
		const auto &PlayerHUD = PlayerController->MyHUD;
		const auto &PlayerState = PlayerController->PlayerState;
		//
		Savior->LoadActor(Savior->GetWorld(),PlayerController,Result);
		//
		if (Pawn!=nullptr) {Savior->LoadActor(Savior->GetWorld(),Pawn,Result);}
		if ((PlayerHUD!=nullptr)&&PlayerHUD->IsValidLowLevelFast()) {Savior->LoadActor(Savior->GetWorld(),PlayerHUD,Result);}
		if ((PlayerState!=nullptr)&&PlayerState->IsValidLowLevelFast()) {Savior->LoadActor(Savior->GetWorld(),PlayerState,Result);}
	}///
	//
	//
	Result = ESaviorResult::Success;
	if (IsInGameThread()) {OnFinishDeserializeGameMode(Savior,Result);} else {
		FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
			FSimpleDelegateGraphTask::FDelegate::CreateStatic(&OnFinishDeserializeGameMode,Savior,Result),
			GET_STATID(STAT_FSimpleDelegateGraphTask_DeserializeGameMode),
			nullptr, ENamedThreads::GameThread
		);//
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Savior Threading Interface [Game-Instance]:

static void OnFinishSerializeGameInstance(USavior2* Savior, ESaviorResult Result) {
	check(IsInGameThread());
	//
	const auto &Settings = GetDefault<USavior2Settings>();
	Savior->WriteSlotToFile(Settings->DefaultPlayerID,Result);
	//
	Savior->EVENT_OnFinishDataSAVE.Broadcast(Result==ESaviorResult::Success);
	Savior->ClearWorkload();
}

///	(C) 2018 - Bruno Xavier B. Leite
static void SerializeGameInstance(USavior2* Savior) {
	if (Savior==nullptr||Savior->GetWorld()==nullptr) {return;}
	const auto &Settings = GetDefault<USavior2Settings>();
	ESaviorResult Result = ESaviorResult::Failed;
	//
	//
	const auto &GameInstance = Savior->GetWorld()->GetGameInstance();
	if ((GameInstance!=nullptr)&&GameInstance->IsValidLowLevelFast()) {
		Savior->SaveGameInstanceSingleTon(GameInstance,Result);
	}///
	//
	//
	if (IsInGameThread()) {OnFinishSerializeGameInstance(Savior,Result);} else {
		FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
			FSimpleDelegateGraphTask::FDelegate::CreateStatic(&OnFinishSerializeGameInstance,Savior,Result),
			GET_STATID(STAT_FSimpleDelegateGraphTask_SerializeGameInstance),
			nullptr, ENamedThreads::GameThread
		);//
	}///
}

static void OnFinishDeserializeGameInstance(USavior2* Savior, ESaviorResult Result) {
	check(IsInGameThread());
	//
	Savior->EVENT_OnFinishDataLOAD.Broadcast(Result==ESaviorResult::Success);
	Savior->ClearWorkload();
}

///	(C) 2018 - Bruno Xavier B. Leite
static void DeserializeGameInstance(USavior2* Savior) {
	if (Savior==nullptr||Savior->GetWorld()==nullptr) {return;}
	const auto &Settings = GetDefault<USavior2Settings>();
	//
	//
	ESaviorResult Result = ESaviorResult::Failed;
	const auto &GameInstance = Savior->GetWorld()->GetGameInstance();
	if ((GameInstance!=nullptr)&&GameInstance->IsValidLowLevelFast()) {
		Savior->LoadGameInstanceSingleTon(GameInstance,Result);
	}///
	//
	//
	if (IsInGameThread()) {OnFinishDeserializeGameInstance(Savior,Result);} else {
		FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
			FSimpleDelegateGraphTask::FDelegate::CreateStatic(&OnFinishDeserializeGameInstance,Savior,Result),
			GET_STATID(STAT_FSimpleDelegateGraphTask_DeserializeGameInstance),
			nullptr, ENamedThreads::GameThread
		);//
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Savior Threading Interface [Entities]:

///	(C) 2018 - Bruno Xavier B. Leite
static void DeserializeActor(const FSaviorRecord Record, AActor* Actor) {
	const auto &Scene = Cast<USceneComponent>(Actor->GetRootComponent());
	const auto &Capsule = Cast<UCapsuleComponent>(Actor->GetRootComponent());
	//
	if (!TAG_ANOTRAN(Actor) && !Record.IgnoreTransform && ((Scene && Scene->Mobility==EComponentMobility::Movable)||(Capsule && Capsule->Mobility==EComponentMobility::Movable))) {
		Actor->SetActorScale3D(Record.Scale);
		Actor->SetActorRotation(Record.Rotation,ETeleportType::TeleportPhysics);
		Actor->SetActorLocation(Record.Location,false,nullptr,ETeleportType::TeleportPhysics);
	}///
	//
	if (!TAG_ANOPHYS(Actor) && Scene && (Scene->Mobility==EComponentMobility::Movable)) {
		Scene->ComponentVelocity = Record.LinearVelocity;
		//
		const auto &Primitive = Cast<UPrimitiveComponent>(Actor->GetRootComponent());
		if (Primitive) {Primitive->SetPhysicsAngularVelocityInDegrees(Record.AngularVelocity);}
	}///
	//
	if (!TAG_ANOHIDE(Actor)) {Actor->SetActorHiddenInGame(Record.HiddenInGame);}
}

///	(C) 2018 - Bruno Xavier B. Leite
static void DeserializeComponent(const FSaviorRecord Record, UActorComponent* Component) {
	const auto &Scene = Cast<USceneComponent>(Component);
	Component->SetActive(Record.Active);
	//
	if (!TAG_CNOTRAN(Component) && (Scene && Scene->Mobility==EComponentMobility::Movable)) {
		Scene->SetRelativeScale3D(Record.Scale);
		Scene->SetRelativeRotation(Record.Rotation,false,nullptr,ETeleportType::TeleportPhysics);
		Scene->SetRelativeLocation(Record.Location,false,nullptr,ETeleportType::TeleportPhysics);
	}///
	//
	const auto &Particle = Cast<UParticleSystemComponent>(Component);
	if (Particle && Record.Active) {Particle->ActivateSystem();}
	else if (Particle) {Particle->DeactivateSystem();}
}

///	(C) 2018 - Bruno Xavier B. Leite
static void DeserializeActor(const FSaviorMinimal Record, AActor* Actor) {
	const auto &Scene = Cast<USceneComponent>(Actor->GetRootComponent());
	const auto &Capsule = Cast<UCapsuleComponent>(Actor->GetRootComponent());
	//
	if (!TAG_ANOTRAN(Actor) && ((Scene && Scene->Mobility==EComponentMobility::Movable)||(Capsule && Capsule->Mobility==EComponentMobility::Movable))) {
		Actor->SetActorRotation(Record.Rotation,ETeleportType::TeleportPhysics);
		Actor->SetActorLocation(Record.Location,false,nullptr,ETeleportType::TeleportPhysics);
	}///
}

///	(C) 2018 - Bruno Xavier B. Leite
static void DeserializeComponent(const FSaviorMinimal Record, UActorComponent* Component) {
	const auto &Scene = Cast<USceneComponent>(Component);
	//
	if (!TAG_CNOTRAN(Component) && (Scene && Scene->Mobility==EComponentMobility::Movable)) {
		Scene->SetRelativeRotation(Record.Rotation,false,nullptr,ETeleportType::TeleportPhysics);
		Scene->SetRelativeLocation(Record.Location,false,nullptr,ETeleportType::TeleportPhysics);
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Savior Threading Task Interface:

class TASK_SerializeGameWorld : public FNonAbandonableTask {
private:
	USavior2* Savior;
public:
	TASK_SerializeGameWorld(USavior2* Savior) {this->Savior = Savior;}
	//
	FORCEINLINE TStatId GetStatId() const {
		RETURN_QUICK_DECLARE_CYCLE_STAT(TASK_SerializeGameWorld,STATGROUP_ThreadPoolAsyncTasks);
	}///
	//
	void DoWork() {SerializeGameWorld(Savior);}
};

class TASK_DeserializeGameWorld : public FNonAbandonableTask {
private:
	USavior2* Savior;
public:
	TASK_DeserializeGameWorld(USavior2* Savior) {this->Savior = Savior;}
	//
	FORCEINLINE TStatId GetStatId() const {
		RETURN_QUICK_DECLARE_CYCLE_STAT(TASK_DeserializeGameWorld,STATGROUP_ThreadPoolAsyncTasks);
	}///
	//
	void DoWork() {DeserializeGameWorld(Savior);}
};

class TASK_SerializeLevel : public FNonAbandonableTask {
private:
	USavior2* Savior;
	FName Level;
public:
	TASK_SerializeLevel(USavior2* Savior, const FName LevelToSave) {this->Savior = Savior; this->Level = LevelToSave;}
	//
	FORCEINLINE TStatId GetStatId() const {
		RETURN_QUICK_DECLARE_CYCLE_STAT(TASK_SerializeLevel,STATGROUP_ThreadPoolAsyncTasks);
	}///
	//
	void DoWork() {SerializeLevel(Savior,Level);}
};

class TASK_DeserializeLevel : public FNonAbandonableTask {
private:
	USavior2* Savior;
	FName Level;
public:
	TASK_DeserializeLevel(USavior2* Savior, const FName LevelToLoad) {this->Savior = Savior; this->Level = LevelToLoad;}
	//
	FORCEINLINE TStatId GetStatId() const {
		RETURN_QUICK_DECLARE_CYCLE_STAT(TASK_DeserializeLevel,STATGROUP_ThreadPoolAsyncTasks);
	}///
	//
	void DoWork() {DeserializeLevel(Savior,Level);}
};

class TASK_SerializeGameMode : public FNonAbandonableTask {
private:
	USavior2* Savior;
public:
	TASK_SerializeGameMode(USavior2* Savior) {this->Savior = Savior;}
	//
	FORCEINLINE TStatId GetStatId() const {
		RETURN_QUICK_DECLARE_CYCLE_STAT(TASK_SerializeGameMode,STATGROUP_ThreadPoolAsyncTasks);
	}///
	//
	void DoWork() {SerializeGameMode(Savior);}
};

class TASK_DeserializeGameMode : public FNonAbandonableTask {
private:
	USavior2* Savior;
public:
	TASK_DeserializeGameMode(USavior2* Savior) {this->Savior = Savior;}
	//
	FORCEINLINE TStatId GetStatId() const {
		RETURN_QUICK_DECLARE_CYCLE_STAT(TASK_DeserializeGameMode,STATGROUP_ThreadPoolAsyncTasks);
	}///
	//
	void DoWork() {DeserializeGameMode(Savior);}
};

class TASK_SerializeGameInstance : public FNonAbandonableTask {
private:
	USavior2* Savior;
public:
	TASK_SerializeGameInstance(USavior2* Savior) {this->Savior = Savior;}
	//
	FORCEINLINE TStatId GetStatId() const {
		RETURN_QUICK_DECLARE_CYCLE_STAT(TASK_SerializeGameInstance,STATGROUP_ThreadPoolAsyncTasks);
	}///
	//
	void DoWork() {SerializeGameInstance(Savior);}
};

class TASK_DeserializeGameInstance : public FNonAbandonableTask {
private:
	USavior2* Savior;
public:
	TASK_DeserializeGameInstance(USavior2* Savior) {this->Savior = Savior;}
	//
	FORCEINLINE TStatId GetStatId() const {
		RETURN_QUICK_DECLARE_CYCLE_STAT(TASK_DeserializeGameInstance,STATGROUP_ThreadPoolAsyncTasks);
	}///
	//
	void DoWork() {DeserializeGameInstance(Savior);}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////