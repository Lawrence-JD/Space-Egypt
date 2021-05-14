//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//			Copyright 2020 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SaviorTypes.h"
#include "Runtime/Engine/Classes/GameFramework/SaveGame.h"

#include "SaviorMetaData.generated.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UCLASS(ClassGroup=Synaptech, Category="Synaptech", BlueprintType, meta=(DisplayName="[SAVIOR] Slot Meta-Data"))
class SAVIOR3_API USlotMeta : public USaveGame {
	GENERATED_BODY()
	//
	USlotMeta();
	friend class USavior3;
public:
	/// :: NATIVE ::
	///
	friend inline FArchive &operator << (FArchive &AR, USlotMeta &SL) {
		AR << SL.Chapter;
		AR << SL.Progress;
		AR << SL.PlayTime;
		AR << SL.SaveDate;
		AR << SL.PlayerName;
		AR << SL.PlayerLevel;
		AR << SL.SaveLocation;
	return AR;}
	///
	/// :: META-DATA ::
	///
	UPROPERTY(Category="Meta Data", EditDefaultsOnly, BlueprintReadWrite) float Progress;
	UPROPERTY(Category="Meta Data", EditDefaultsOnly, BlueprintReadWrite) int32 PlayTime;
	UPROPERTY(Category="Meta Data", EditDefaultsOnly, BlueprintReadWrite) FString Chapter;
	UPROPERTY(Category="Meta Data", EditDefaultsOnly, BlueprintReadWrite) int32 PlayerLevel;
	UPROPERTY(Category="Meta Data", EditDefaultsOnly, BlueprintReadWrite) FString PlayerName;
	UPROPERTY(Category="Meta Data", EditDefaultsOnly, BlueprintReadWrite) FString SaveLocation;
	UPROPERTY(Category="Meta Data", VisibleDefaultsOnly, BlueprintReadWrite) FDateTime SaveDate;
};

USTRUCT(BlueprintType,meta=(DisplayName="Slot Meta-Data"))
struct SAVIOR3_API FSlotMetaData {
	GENERATED_USTRUCT_BODY()
	//
	UPROPERTY(Category="Meta Data", BlueprintReadOnly) float Progress;
	UPROPERTY(Category="Meta Data", BlueprintReadOnly) int32 PlayTime;
	UPROPERTY(Category="Meta Data", BlueprintReadOnly) FString Chapter;
	UPROPERTY(Category="Meta Data", BlueprintReadOnly) int32 PlayerLevel;
	UPROPERTY(Category="Meta Data", BlueprintReadOnly) FString PlayerName;
	UPROPERTY(Category="Meta Data", BlueprintReadOnly) FDateTime SaveDate;
	UPROPERTY(Category="Meta Data", BlueprintReadOnly) FString SaveLocation;
	//
	//
	FSlotMetaData()
		: Progress(0.f)
		, PlayTime(0)
		, Chapter(TEXT(""))
		, PlayerLevel(0)
		, PlayerName(TEXT(""))
		, SaveDate(FDateTime::Now())
		, SaveLocation(TEXT(""))
	{}///
	//
	FSlotMetaData(USlotMeta* SM)
		: Progress(0.f)
		, PlayTime(0)
		, Chapter(TEXT(""))
		, PlayerLevel(0)
		, PlayerName(TEXT(""))
		, SaveDate(FDateTime::Now())
		, SaveLocation(TEXT(""))
	{///
		if (SM->IsValidLowLevelFast()) {
			Chapter = SM->Chapter;
			Progress = SM->Progress;
			PlayTime = SM->PlayTime;
			SaveDate = SM->SaveDate;
			PlayerName = SM->PlayerName;
			PlayerLevel = SM->PlayerLevel;
			SaveLocation = SM->SaveLocation;
		}///
	}///
	//
	FSlotMetaData(const USlotMeta* SM)
		: Progress(0.f)
		, PlayTime(0)
		, Chapter(TEXT(""))
		, PlayerLevel(0)
		, PlayerName(TEXT(""))
		, SaveDate(FDateTime::Now())
		, SaveLocation(TEXT(""))
	{///
		if (SM->IsValidLowLevelFast()) {
			Chapter = SM->Chapter;
			Progress = SM->Progress;
			PlayTime = SM->PlayTime;
			SaveDate = SM->SaveDate;
			PlayerName = SM->PlayerName;
			PlayerLevel = SM->PlayerLevel;
			SaveLocation = SM->SaveLocation;
		}///
	}///
	//
	//
	FORCEINLINE FSlotMetaData &operator = (const FSlotMetaData &SM) {
		Chapter = SM.Chapter;
		Progress = SM.Progress;
		PlayTime = SM.PlayTime;
		SaveDate = SM.SaveDate;
		PlayerName = SM.PlayerName;
		PlayerLevel = SM.PlayerLevel;
		SaveLocation = SM.SaveLocation;
	return *this;}
	//
	FORCEINLINE FSlotMetaData &operator = (USlotMeta* SM) {
		Chapter = SM->Chapter;
		Progress = SM->Progress;
		PlayTime = SM->PlayTime;
		SaveDate = SM->SaveDate;
		PlayerName = SM->PlayerName;
		PlayerLevel = SM->PlayerLevel;
		SaveLocation = SM->SaveLocation;
	return *this;}
	//
	FORCEINLINE FSlotMetaData &operator = (const USlotMeta* SM) {
		Chapter = SM->Chapter;
		Progress = SM->Progress;
		PlayTime = SM->PlayTime;
		SaveDate = SM->SaveDate;
		PlayerName = SM->PlayerName;
		PlayerLevel = SM->PlayerLevel;
		SaveLocation = SM->SaveLocation;
	return *this;}
	//
	//
	friend FORCEINLINE uint32 GetTypeHash(const FSlotMetaData &SM) {
		return FCrc::MemCrc32(&SM,sizeof(FSlotMetaData));
	}///
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////