//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//			Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Savior2_Shared.h"

#include "Runtime/Core/Public/Serialization/ArchiveSaveCompressedProxy.h"
#include "Runtime/Core/Public/Serialization/ArchiveLoadCompressedProxy.h"

#include "SaviorTypes.generated.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class USavior2;
class USlotMeta;
class SaviorReflector;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Savior 2 Internal Classes:

enum class ESeverity : uint8 {
	Fatal			= 0,
	Error			= 1,
	Warning			= 2,
	Info			= 3
};

UENUM(BlueprintType, meta=(DisplayName="[SAVIOR] Runtime Mode", RestrictedToClasses=Savior2, BlueprintInternalUseOnly=true))
enum class ERuntimeMode : uint8 {
	/// Executes Serialization within Game Thread.
	SynchronousTask,
	/// Executes Serialization from its own Asynchronous Threaded.
	BackgroundTask
};

UENUM(BlueprintType, meta=(DisplayName="[SAVIOR] Thread Safety", RestrictedToClasses=Savior2, BlueprintInternalUseOnly=true))
enum class EThreadSafety : uint8 {
	/// Is Safe to Bind any Interface Calls.
	IsCurrentlyThreadSafe,
	/// Is about to perform runtime expensive operations.
	IsPreparingToSaveOrLoad,
	/// Is Unsafe Loading. Any interaction with Main Game's Thread will crash!
	AsynchronousLoading,
	/// Is Unsafe Saving. Any interaction with Main Game's Thread will crash!
	AsynchronousSaving,
	/// Is Safely Loading. Any interaction with Main Game's Thread is safe!
	SynchronousLoading,
	/// Is Safely Saving. Any interaction with Main Game's Thread is safe!
	SynchronousSaving
};

UENUM(BlueprintType,meta=(DisplayName="[SAVIOR] Serializer Type"))
enum class ERecordType : uint8 {
	Complex,
	Minimal
};

UENUM(BlueprintType,meta=(DisplayName="[SAVIOR] Serializer Result"))
enum class ESaviorResult : uint8 {
	Success,
	Failed
};

UENUM(meta=(RestrictedToClasses=SaviorReflector,BlueprintInternalUseOnly=true))
enum class EStructType : uint8 {
	Struct,
	Transform,
	Vector2D,
	Vector3D,
	Rotator,
	LColor,
	FColor,
	Curve
};

UENUM(BlueprintType, meta=(DisplayName="[SAVIOR] Load-Screen Mode", RestrictedToClasses=Savior2, BlueprintInternalUseOnly=true))
enum class ELoadScreenMode : uint8 {
	BackgroundBlur,
	SplashScreen,
	MoviePlayer,
	NoLoadScreen
};

UENUM(BlueprintType, meta=(DisplayName="[SAVIOR] Load-Screen Trigger", RestrictedToClasses=Savior2, BlueprintInternalUseOnly=true))
enum class ELoadScreenTrigger : uint8 {
	AllScreens,
	WhileSaving,
	WhileLoading
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Savior 2 Internal Structures:

USTRUCT(BlueprintType,meta=(RestrictedToClasses=Savior2,BlueprintInternalUseOnly=true))
struct SAVIOR2_API FSaviorMinimal {
	GENERATED_USTRUCT_BODY()
	//
	//
	UPROPERTY() FString Buffer;
	UPROPERTY() FVector Location;
	UPROPERTY() FRotator Rotation;
	//
	UPROPERTY() bool Destroyed;
	//
	//
	inline FSaviorMinimal &operator = (const FSaviorMinimal &REC) {
		Buffer = REC.Buffer;
		Location = REC.Location;
		Rotation = REC.Rotation;
		Destroyed = REC.Destroyed;
	return *this;}
	//
	friend inline uint32 GetTypeHash(const FSaviorMinimal &REC) {
		return FCrc::MemCrc32(&REC,sizeof(FSaviorMinimal));
	}///
	//
	//
	FSaviorMinimal()
		: Buffer(TEXT(""))
		, Location(FVector::ZeroVector)
		, Rotation(FRotator::ZeroRotator)
		, Destroyed(false)
	{}///
};

USTRUCT(BlueprintType,meta=(RestrictedToClasses=Savior2,BlueprintInternalUseOnly=true))
struct SAVIOR2_API FSaviorRecord {
	GENERATED_USTRUCT_BODY()
	//
	//
	UPROPERTY() FGuid GUID;
	//
	UPROPERTY() FString FullName;
	UPROPERTY() FString OuterName;
	UPROPERTY() FString ClassPath;
	//
	UPROPERTY() FString Buffer;
	UPROPERTY() TArray<FName>Tags;
	//
	UPROPERTY() FVector Scale;
	UPROPERTY() FVector Location;
	UPROPERTY() FRotator Rotation;
	//
	UPROPERTY() FVector LinearVelocity;
	UPROPERTY() FVector AngularVelocity;
	//
	UPROPERTY() bool Active;
	UPROPERTY() bool Destroyed;
	UPROPERTY() bool HiddenInGame;
	UPROPERTY(Transient) bool IgnoreTransform;
	//
	//
	FSaviorRecord()
		: GUID()
		//
		, FullName(TEXT(""))
		, OuterName(TEXT(""))
		, ClassPath(TEXT(""))
		//
		, Buffer(TEXT(""))
		//
		, Scale(FVector::ZeroVector)
		, Location(FVector::ZeroVector)
		, Rotation(FRotator::ZeroRotator)
		//
		, LinearVelocity(FVector::ZeroVector)
		, AngularVelocity(FVector::ZeroVector)
		//
		, Active(true)
		, Destroyed(false)
		, HiddenInGame(false)
		, IgnoreTransform(false)
	{}///
	//
	//
	inline FSaviorRecord &operator = (const FSaviorRecord &REC) {
		GUID = REC.GUID;
		Tags = REC.Tags;
		Scale = REC.Scale;
		Buffer = REC.Buffer;
		FullName = REC.FullName;
		Location = REC.Location;
		Rotation = REC.Rotation;
		ClassPath = REC.ClassPath;
		Destroyed = REC.Destroyed;
		HiddenInGame = REC.HiddenInGame;
		LinearVelocity = REC.LinearVelocity;
		AngularVelocity = REC.AngularVelocity;
	return *this;}
	//
	//
	friend inline uint32 GetTypeHash(const FSaviorRecord &REC) {
		return FCrc::MemCrc32(&REC,sizeof(FSaviorRecord));
	}///
};

USTRUCT(meta=(RestrictedToClasses=Savior2,BlueprintInternalUseOnly=true))
struct SAVIOR2_API FSaviorRedirector {
	GENERATED_USTRUCT_BODY()
	//
	UPROPERTY(Category="Versioning", EditAnywhere)
	TMap<FName,FName> PropertyRedirect;
	//
	//
	inline FSaviorRedirector &operator = (const FSaviorRedirector &RED) {
		PropertyRedirect = RED.PropertyRedirect;
	return *this;}
	//
	friend inline uint32 GetTypeHash(const FSaviorRedirector &RED) {
		return FCrc::MemCrc32(&RED,sizeof(FSaviorRedirector));
	}///
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Savior 2 Internal Operators:

inline FArchive &operator << (FArchive &AR, FSaviorMinimal &SR) {
	AR << SR.Buffer;
	AR << SR.Location;
	AR << SR.Rotation;
	AR << SR.Destroyed;
	//
	return AR;
}

inline FArchive &operator << (FArchive &AR, FSaviorRecord &SR) {
	AR << SR.GUID;
	AR << SR.Tags;
	AR << SR.Scale;
	AR << SR.Buffer;
	AR << SR.FullName;
	AR << SR.Location;
	AR << SR.Rotation;
	AR << SR.ClassPath;
	AR << SR.Destroyed;
	AR << SR.HiddenInGame;
	AR << SR.LinearVelocity;
	AR << SR.AngularVelocity;
	//
	return AR;
}

inline FArchive &operator << (FArchive &AR, FSaviorRedirector &SR) {
	AR << SR.PropertyRedirect;
	//
	return AR;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////