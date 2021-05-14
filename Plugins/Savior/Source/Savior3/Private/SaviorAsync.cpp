//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//			Copyright 2020 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SaviorAsync.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Open Level:

USAVIOR_OpenLevel* USAVIOR_OpenLevel::OpenLevel(UObject* Context, AHUD_SaviorUI* HUD, USavior3* Slot, TSoftObjectPtr<UWorld>LevelToOpen) {
	USAVIOR_OpenLevel* OBJ = NewObject<USAVIOR_OpenLevel>();
	//
	FString Name;
	LevelToOpen.ToString().Split(TEXT("."),nullptr,&Name,ESearchCase::IgnoreCase,ESearchDir::FromEnd);
	//
	OBJ->Level = (LevelToOpen.IsNull()) ? TEXT("None") : *Name;
	OBJ->Outer = Context;
	OBJ->Target = Slot;
	OBJ->SHUD = HUD;
	//
	return OBJ;
}

void USAVIOR_OpenLevel::Activate() {
	if (Target==nullptr||!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	if (SHUD==nullptr||SHUD->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target HUD."),ELogVerbosity::Error); return;}
	//
	Execute();
}

void USAVIOR_OpenLevel::Execute() {
	switch (Target->LoadScreenMode) {
		case ELoadScreenMode::BackgroundBlur:
		{
			SHUD->DisplayBlurLoadScreenHUD(EThreadSafety::SynchronousLoading,Target->FeedbackLOAD,Target->FeedbackFont,Target->ProgressBarTint,Target->BackBlurPower);
		}	break;
		//
		case ELoadScreenMode::SplashScreen:
		{
			SHUD->DisplaySplashLoadScreenHUD(EThreadSafety::SynchronousLoading,Target->FeedbackLOAD,Target->FeedbackFont,Target->ProgressBarTint,Target->SplashImage,Target->SplashStretch);
		}	break;
		//
		case ELoadScreenMode::MoviePlayer:
		{
			SHUD->DisplayMovieLoadScreenHUD(EThreadSafety::SynchronousLoading,Target->FeedbackLOAD,Target->FeedbackFont,Target->ProgressBarTint,Target->SplashMovie,Target->ProgressBarOnMovie);
		}	break;
	default: break;}
	//
	UGameplayStatics::OpenLevel(Outer,Level);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Open Level [+Callbacks]:

USAVIOR_OpenLevel_Callback* USAVIOR_OpenLevel_Callback::OpenLevel(UObject* Context, AHUD_SaviorUI* HUD, USavior3* Slot, TSoftObjectPtr<UWorld>LevelToOpen, const FBeginDATA &OnOpenCallback) {
	USAVIOR_OpenLevel_Callback* OBJ = NewObject<USAVIOR_OpenLevel_Callback>();
	//
	FString Name;
	LevelToOpen.ToString().Split(TEXT("."),nullptr,&Name,ESearchCase::IgnoreCase,ESearchDir::FromEnd);
	//
	OBJ->OnOpenLevel = OnOpenCallback;
	//
	OBJ->Level = (LevelToOpen.IsNull()) ? TEXT("None") : *Name;
	OBJ->Outer = Context;
	OBJ->Target = Slot;
	OBJ->SHUD = HUD;
	//
	return OBJ;
}

void USAVIOR_OpenLevel_Callback::Activate() {
	if (Target==nullptr||!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	if (SHUD==nullptr||SHUD->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target HUD."),ELogVerbosity::Error); return;}
	//
	Execute();
}

void USAVIOR_OpenLevel_Callback::Execute() {
	switch (Target->LoadScreenMode) {
		case ELoadScreenMode::BackgroundBlur:
		{
			SHUD->DisplayBlurLoadScreenHUD(EThreadSafety::SynchronousLoading,Target->FeedbackLOAD,Target->FeedbackFont,Target->ProgressBarTint,Target->BackBlurPower);
		}	break;
		//
		case ELoadScreenMode::SplashScreen:
		{
			SHUD->DisplaySplashLoadScreenHUD(EThreadSafety::SynchronousLoading,Target->FeedbackLOAD,Target->FeedbackFont,Target->ProgressBarTint,Target->SplashImage,Target->SplashStretch);
		}	break;
		//
		case ELoadScreenMode::MoviePlayer:
		{
			SHUD->DisplayMovieLoadScreenHUD(EThreadSafety::SynchronousLoading,Target->FeedbackLOAD,Target->FeedbackFont,Target->ProgressBarTint,Target->SplashMovie,Target->ProgressBarOnMovie);
		}	break;
	default: break;}
	//
	OnOpenLevel.ExecuteIfBound();
	//
	UGameplayStatics::OpenLevel(Outer,Level);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Save Game World:

USAVIOR_SaveGameWorld* USAVIOR_SaveGameWorld::SaveGameWorld(USavior3* Savior, UObject* Context, const bool SaveSlotMeta) {
	USAVIOR_SaveGameWorld* OBJ = NewObject<USAVIOR_SaveGameWorld>();
	ESaviorResult Result;
	//
	OBJ->Target = USavior3::NewSlotInstance(Context->GetWorld(),Savior,Result);
	//
	if (OBJ->Target!=nullptr) {
		OBJ->Target->EVENT_OnFinishDataSAVE.AddDynamic(OBJ,&USAVIOR_SaveGameWorld::Finish);
		OBJ->Target->WriteMetaOnSave = SaveSlotMeta;
		OBJ->Outer = Context;
	}///
	//
	return OBJ;
}

void USAVIOR_SaveGameWorld::Activate() {
	if (Target==nullptr||!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	//
	Execute();
}

void USAVIOR_SaveGameWorld::Execute() {
	ESaviorResult Result;
	//
	Target->SaveGameWorld(Outer,Result);
}

void USAVIOR_SaveGameWorld::Finish(const bool Succeeded) {
	if (Succeeded) {
		OnSuccess.Broadcast();
	} else {
		OnFail.Broadcast();
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Save Game World [+Callbacks]:

USAVIOR_SaveGameWorld_Callback* USAVIOR_SaveGameWorld_Callback::SaveGameWorld(USavior3* Savior, UObject* Context, const bool SaveSlotMeta, const FBeginDATA &BeganSaveCallback, const FEndDATA &FinishedSaveCallback) {
	USAVIOR_SaveGameWorld_Callback* OBJ = NewObject<USAVIOR_SaveGameWorld_Callback>();
	ESaviorResult Result;
	//
	OBJ->BeganSave = BeganSaveCallback;
	OBJ->FinishedSave = FinishedSaveCallback;
	//
	OBJ->Target = USavior3::NewSlotInstance(Context->GetWorld(),Savior,Result);
	//
	if (OBJ->Target!=nullptr) {
		OBJ->Target->EVENT_OnFinishDataSAVE.AddDynamic(OBJ,&USAVIOR_SaveGameWorld_Callback::Finish);
		OBJ->Target->WriteMetaOnSave = SaveSlotMeta;
		OBJ->Outer = Context;
	}///
	//
	return OBJ;
}

void USAVIOR_SaveGameWorld_Callback::Activate() {
	if (Target==nullptr||!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	//
	Execute();
}

void USAVIOR_SaveGameWorld_Callback::Execute() {
	BeganSave.ExecuteIfBound();
	ESaviorResult Result;
	//
	Target->SaveGameWorld(Outer,Result);
}

void USAVIOR_SaveGameWorld_Callback::Finish(const bool Succeeded) {
	if (Succeeded) {
		OnSuccess.Broadcast();
	} else {
		OnFail.Broadcast();
	}///
	//
	FinishedSave.ExecuteIfBound(Succeeded);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load Game World:

USAVIOR_LoadGameWorld* USAVIOR_LoadGameWorld::LoadGameWorld(USavior3* Savior, UObject* Context, const bool ResetLevelOnLoad) {
	USAVIOR_LoadGameWorld* OBJ = NewObject<USAVIOR_LoadGameWorld>();
	ESaviorResult Result;
	//
	OBJ->Target = USavior3::NewSlotInstance(Context->GetWorld(),Savior,Result);
	//
	if (OBJ->Target!=nullptr) {
		OBJ->Target->EVENT_OnFinishDataLOAD.AddDynamic(OBJ,&USAVIOR_LoadGameWorld::Finish);
		OBJ->ResetLevel = ResetLevelOnLoad;
		OBJ->Outer = Context;
	}///
	//
	return OBJ;
}

void USAVIOR_LoadGameWorld::Activate() {
	if (Target==nullptr||!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	//
	Execute();
}

void USAVIOR_LoadGameWorld::Execute() {
	ESaviorResult Result;
	//
	Target->LoadGameWorld(Outer,ResetLevel,Result);
}

void USAVIOR_LoadGameWorld::Finish(const bool Succeeded) {
	if (Succeeded) {
		OnSuccess.Broadcast();
	} else {
		OnFail.Broadcast();
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load Game World [+Callbacks]:

USAVIOR_LoadGameWorld_Callback* USAVIOR_LoadGameWorld_Callback::LoadGameWorld(USavior3* Savior, UObject* Context, const bool ResetLevelOnLoad, const FBeginDATA &BeganLoadCallback, const FEndDATA &FinishedLoadCallback) {
	USAVIOR_LoadGameWorld_Callback* OBJ = NewObject<USAVIOR_LoadGameWorld_Callback>();
	ESaviorResult Result;
	//
	OBJ->BeganLoad = BeganLoadCallback;
	OBJ->FinishedLoad = FinishedLoadCallback;
	OBJ->Target = USavior3::NewSlotInstance(Context->GetWorld(),Savior,Result);
	//
	if (OBJ->Target!=nullptr) {
		OBJ->Target->EVENT_OnFinishDataLOAD.AddDynamic(OBJ,&USAVIOR_LoadGameWorld_Callback::Finish);
		OBJ->ResetLevel = ResetLevelOnLoad;
		OBJ->Outer = Context;
	}///
	//
	return OBJ;
}

void USAVIOR_LoadGameWorld_Callback::Activate() {
	if (Target==nullptr||!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	//
	Execute();
}

void USAVIOR_LoadGameWorld_Callback::Execute() {
	BeganLoad.ExecuteIfBound();
	ESaviorResult Result;
	//
	Target->LoadGameWorld(Outer,ResetLevel,Result);
}

void USAVIOR_LoadGameWorld_Callback::Finish(const bool Succeeded) {
	if (Succeeded) {
		OnSuccess.Broadcast();
	} else {
		OnFail.Broadcast();
	}///
	//
	FinishedLoad.ExecuteIfBound(Succeeded);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Save Level:

USAVIOR_SaveLevel* USAVIOR_SaveLevel::SaveLevel(USavior3* Savior, TSoftObjectPtr<UWorld>LevelToSave, UObject* Context, const bool SaveSlotMeta) {
	USAVIOR_SaveLevel* OBJ = NewObject<USAVIOR_SaveLevel>();
	ESaviorResult Result;
	//
	FString Name;
	LevelToSave.ToString().Split(TEXT("."),nullptr,&Name,ESearchCase::IgnoreCase,ESearchDir::FromEnd);
	//
	OBJ->Target = USavior3::NewSlotInstance(Context->GetWorld(),Savior,Result);
	//
	if (OBJ->Target!=nullptr) {
		OBJ->Target->EVENT_OnFinishDataSAVE.AddDynamic(OBJ,&USAVIOR_SaveLevel::Finish);
		OBJ->Target->WriteMetaOnSave = SaveSlotMeta;
		OBJ->Outer = Context;
	}///
	//
	OBJ->Level = (LevelToSave.IsNull()) ? TEXT("None") : *Name;
	//
	return OBJ;
}

void USAVIOR_SaveLevel::Activate() {
	if (Target==nullptr||!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	//
	Execute();
}

void USAVIOR_SaveLevel::Execute() {
	ESaviorResult Result;
	//
	Target->SaveLevel(Outer,Level,Result);
}

void USAVIOR_SaveLevel::Finish(const bool Succeeded) {
	if (Succeeded) {
		OnSuccess.Broadcast();
	} else {
		OnFail.Broadcast();
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Save Level [+Callbacks]:

USAVIOR_SaveLevel_Callback* USAVIOR_SaveLevel_Callback::SaveLevel(USavior3* Savior, TSoftObjectPtr<UWorld>LevelToSave, UObject* Context, const bool SaveSlotMeta, const FBeginDATA &BeganSaveCallback, const FEndDATA &FinishedSaveCallback) {
	USAVIOR_SaveLevel_Callback* OBJ = NewObject<USAVIOR_SaveLevel_Callback>();
	ESaviorResult Result;
	//
	FString Name;
	LevelToSave.ToString().Split(TEXT("."),nullptr,&Name,ESearchCase::IgnoreCase,ESearchDir::FromEnd);
	//
	OBJ->BeganSave = BeganSaveCallback;
	OBJ->FinishedSave = FinishedSaveCallback;
	OBJ->Level = (LevelToSave.IsNull()) ? TEXT("None") : *Name;
	//
	OBJ->Target = USavior3::NewSlotInstance(Context->GetWorld(),Savior,Result);
	//
	if (OBJ->Target!=nullptr) {
		OBJ->Target->EVENT_OnFinishDataSAVE.AddDynamic(OBJ,&USAVIOR_SaveLevel_Callback::Finish);
		OBJ->Target->WriteMetaOnSave = SaveSlotMeta;
		OBJ->Outer = Context;
	}///
	//
	return OBJ;
}

void USAVIOR_SaveLevel_Callback::Activate() {
	if (Target==nullptr||!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	//
	Execute();
}

void USAVIOR_SaveLevel_Callback::Execute() {
	BeganSave.ExecuteIfBound();
	ESaviorResult Result;
	//
	Target->SaveLevel(Outer,Level,Result);
}

void USAVIOR_SaveLevel_Callback::Finish(const bool Succeeded) {
	if (Succeeded) {
		OnSuccess.Broadcast();
	} else {
		OnFail.Broadcast();
	}///
	//
	FinishedSave.ExecuteIfBound(Succeeded);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load Level:

USAVIOR_LoadLevel* USAVIOR_LoadLevel::LoadLevel(USavior3* Savior, TSoftObjectPtr<UWorld>LevelToLoad, UObject* Context, const bool StreamLevelOnLoad) {
	USAVIOR_LoadLevel* OBJ = NewObject<USAVIOR_LoadLevel>();
	ESaviorResult Result;
	//
	FString Name;
	LevelToLoad.ToString().Split(TEXT("."),nullptr,&Name,ESearchCase::IgnoreCase,ESearchDir::FromEnd);
	//
	OBJ->Target = USavior3::NewSlotInstance(Context->GetWorld(),Savior,Result);
	//
	if (OBJ->Target!=nullptr) {
		OBJ->Target->EVENT_OnFinishDataLOAD.AddDynamic(OBJ,&USAVIOR_LoadLevel::Finish);
		OBJ->StreamLevel = StreamLevelOnLoad;
		OBJ->Outer = Context;
	}///
	//
	OBJ->Level = (LevelToLoad.IsNull()) ? TEXT("None") : *Name;
	//
	return OBJ;
}

void USAVIOR_LoadLevel::Activate() {
	if (Target==nullptr||!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	//
	Execute();
}

void USAVIOR_LoadLevel::Execute() {
	ESaviorResult Result;
	//
	if (StreamLevel) {
		FLatentActionInfo LevelInfo;
		UGameplayStatics::LoadStreamLevel(Outer,Level,true,true,LevelInfo);
	}///
	//
	Target->LoadLevel(Outer,Level,Result);
}

void USAVIOR_LoadLevel::Finish(const bool Succeeded) {
	if (Succeeded) {
		OnSuccess.Broadcast();
	} else {
		OnFail.Broadcast();
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load Level [+Callbacks]:

USAVIOR_LoadLevel_Callback* USAVIOR_LoadLevel_Callback::LoadLevel(USavior3* Savior, TSoftObjectPtr<UWorld>LevelToLoad, UObject* Context, const bool StreamLevelOnLoad, const FBeginDATA &BeganLoadCallback, const FEndDATA &FinishedLoadCallback) {
	USAVIOR_LoadLevel_Callback* OBJ = NewObject<USAVIOR_LoadLevel_Callback>();
	ESaviorResult Result;
	//
	FString Name;
	LevelToLoad.ToString().Split(TEXT("."),nullptr,&Name,ESearchCase::IgnoreCase,ESearchDir::FromEnd);
	//
	OBJ->BeganLoad = BeganLoadCallback;
	OBJ->FinishedLoad = FinishedLoadCallback;
	//
	OBJ->Level = (LevelToLoad.IsNull()) ? TEXT("None") : *Name;
	OBJ->Target = USavior3::NewSlotInstance(Context->GetWorld(),Savior,Result);
	//
	if (OBJ->Target!=nullptr) {
		OBJ->Target->EVENT_OnFinishDataLOAD.AddDynamic(OBJ,&USAVIOR_LoadLevel_Callback::Finish);
		OBJ->StreamLevel = StreamLevelOnLoad;
		OBJ->Outer = Context;
	}///
	//
	return OBJ;
}

void USAVIOR_LoadLevel_Callback::Activate() {
	if (Target==nullptr||!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	//
	Execute();
}

void USAVIOR_LoadLevel_Callback::Execute() {
	BeganLoad.ExecuteIfBound();
	ESaviorResult Result;
	//
	if (StreamLevel) {
		FLatentActionInfo LevelInfo;
		UGameplayStatics::LoadStreamLevel(Outer,Level,true,true,LevelInfo);
	}///
	//
	Target->LoadLevel(Outer,Level,Result);
}

void USAVIOR_LoadLevel_Callback::Finish(const bool Succeeded) {
	if (Succeeded) {
		OnSuccess.Broadcast();
	} else {
		OnFail.Broadcast();
	}///
	//
	FinishedLoad.ExecuteIfBound(Succeeded);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Save Game Mode:

USAVIOR_SaveGameMode* USAVIOR_SaveGameMode::SaveGameMode(USavior3* Savior, UObject* Context, const bool SaveSlotMeta) {
	USAVIOR_SaveGameMode* OBJ = NewObject<USAVIOR_SaveGameMode>();
	ESaviorResult Result;
	//
	OBJ->Target = USavior3::NewSlotInstance(Context->GetWorld(),Savior,Result);
	//
	if (OBJ->Target!=nullptr) {
		OBJ->Target->EVENT_OnFinishDataSAVE.AddDynamic(OBJ,&USAVIOR_SaveGameMode::Finish);
		OBJ->Target->WriteMetaOnSave = SaveSlotMeta;
		OBJ->Outer = Context;
	}///
	//
	return OBJ;
}

void USAVIOR_SaveGameMode::Activate() {
	if (!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	//
	Execute();
}

void USAVIOR_SaveGameMode::Execute() {
	ESaviorResult Result;
	//
	Target->SaveGameMode(Outer,Result);
}

void USAVIOR_SaveGameMode::Finish(const bool Succeeded) {
	if (Succeeded) {
		OnSuccess.Broadcast();
	} else {
		OnFail.Broadcast();
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Save Game Mode [+Callbacks]:

USAVIOR_SaveGameMode_Callback* USAVIOR_SaveGameMode_Callback::SaveGameMode(USavior3* Savior, UObject* Context, const bool SaveSlotMeta, const FBeginDATA &BeganSaveCallback, const FEndDATA &FinishedSaveCallback) {
	USAVIOR_SaveGameMode_Callback* OBJ = NewObject<USAVIOR_SaveGameMode_Callback>();
	ESaviorResult Result;
	//
	OBJ->Outer = Context;
	OBJ->BeganSave = BeganSaveCallback;
	OBJ->FinishedSave = FinishedSaveCallback;
	//
	OBJ->Target = USavior3::NewSlotInstance(Context->GetWorld(),Savior,Result);
	//
	if (OBJ->Target!=nullptr) {
		OBJ->Target->EVENT_OnFinishDataSAVE.AddDynamic(OBJ,&USAVIOR_SaveGameMode_Callback::Finish);
		OBJ->Target->WriteMetaOnSave = SaveSlotMeta;
		OBJ->Outer = Context;
	}///
	//
	return OBJ;
}

void USAVIOR_SaveGameMode_Callback::Activate() {
	if (Target==nullptr||!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	//
	Execute();
}

void USAVIOR_SaveGameMode_Callback::Execute() {
	BeganSave.ExecuteIfBound();
	ESaviorResult Result;
	//
	Target->SaveGameMode(Outer,Result);
}

void USAVIOR_SaveGameMode_Callback::Finish(const bool Succeeded) {
	if (Succeeded) {
		OnSuccess.Broadcast();
	} else {
		OnFail.Broadcast();
	}///
	//
	FinishedSave.ExecuteIfBound(Succeeded);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load Game Mode:

USAVIOR_LoadGameMode* USAVIOR_LoadGameMode::LoadGameMode(USavior3* Savior, UObject* Context, const bool IgnoreMainPawnTransform) {
	USAVIOR_LoadGameMode* OBJ = NewObject<USAVIOR_LoadGameMode>();
	ESaviorResult Result;
	//
	OBJ->Target = USavior3::NewSlotInstance(Context->GetWorld(),Savior,Result);
	//
	if (OBJ->Target!=nullptr) {
		OBJ->Target->EVENT_OnFinishDataLOAD.AddDynamic(OBJ,&USAVIOR_LoadGameMode::Finish);
		OBJ->Target->IgnorePawnTransformOnLoad = IgnoreMainPawnTransform;
		OBJ->Outer = Context;
	}///
	//
	return OBJ;
}

void USAVIOR_LoadGameMode::Activate() {
	if (Target==nullptr||!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	//
	Execute();
}

void USAVIOR_LoadGameMode::Execute() {
	ESaviorResult Result;
	//
	Target->LoadGameMode(Outer,Result);
}

void USAVIOR_LoadGameMode::Finish(const bool Succeeded) {
	if (Succeeded) {
		OnSuccess.Broadcast();
	} else {
		OnFail.Broadcast();
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load Game Mode [+Callbacks]:

USAVIOR_LoadGameMode_Callback* USAVIOR_LoadGameMode_Callback::LoadGameMode(USavior3* Savior, UObject* Context, const bool IgnoreMainPawnTransform, const FBeginDATA &BeganLoadCallback, const FEndDATA &FinishedLoadCallback) {
	USAVIOR_LoadGameMode_Callback* OBJ = NewObject<USAVIOR_LoadGameMode_Callback>();
	ESaviorResult Result;
	//
	OBJ->Outer = Context;
	OBJ->BeganLoad = BeganLoadCallback;
	OBJ->FinishedLoad = FinishedLoadCallback;
	//
	OBJ->Target = USavior3::NewSlotInstance(Context->GetWorld(),Savior,Result);
	//
	if (OBJ->Target!=nullptr) {
		OBJ->Target->EVENT_OnFinishDataLOAD.AddDynamic(OBJ,&USAVIOR_LoadGameMode_Callback::Finish);
		OBJ->Target->IgnorePawnTransformOnLoad = IgnoreMainPawnTransform;
		OBJ->Outer = Context;
	}///
	//
	return OBJ;
}

void USAVIOR_LoadGameMode_Callback::Activate() {
	if (Target==nullptr||!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	//
	Execute();
}

void USAVIOR_LoadGameMode_Callback::Execute() {
	BeganLoad.ExecuteIfBound();
	ESaviorResult Result;
	//
	Target->LoadGameMode(Outer,Result);
}

void USAVIOR_LoadGameMode_Callback::Finish(const bool Succeeded) {
	if (Succeeded) {
		OnSuccess.Broadcast();
	} else {
		OnFail.Broadcast();
	}///
	//
	FinishedLoad.ExecuteIfBound(Succeeded);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Save Game Instance:

USAVIOR_SaveGameInstance* USAVIOR_SaveGameInstance::SaveGameInstance(USavior3* Savior, UObject* Context, const bool SaveSlotMeta) {
	USAVIOR_SaveGameInstance* OBJ = NewObject<USAVIOR_SaveGameInstance>();
	ESaviorResult Result;
	//
	OBJ->Target = USavior3::NewSlotInstance(Context->GetWorld(),Savior,Result);
	//
	if (OBJ->Target!=nullptr) {
		OBJ->Target->EVENT_OnFinishDataSAVE.AddDynamic(OBJ,&USAVIOR_SaveGameInstance::Finish);
		OBJ->Target->WriteMetaOnSave = SaveSlotMeta;
		OBJ->Outer = Context;
	}///
	//
	return OBJ;
}

void USAVIOR_SaveGameInstance::Activate() {
	if (Target==nullptr||!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	//
	Execute();
}

void USAVIOR_SaveGameInstance::Execute() {
	ESaviorResult Result;
	//
	Target->SaveGameInstance(Outer,Result);
}

void USAVIOR_SaveGameInstance::Finish(const bool Succeeded) {
	if (Succeeded) {
		OnSuccess.Broadcast();
	} else {
		OnFail.Broadcast();
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Save Game Instance [+Callbacks]:

USAVIOR_SaveGameInstance_Callback* USAVIOR_SaveGameInstance_Callback::SaveGameInstance(USavior3* Savior, UObject* Context, const bool SaveSlotMeta, const FBeginDATA &BeganSaveCallback, const FEndDATA &FinishedSaveCallback) {
	USAVIOR_SaveGameInstance_Callback* OBJ = NewObject<USAVIOR_SaveGameInstance_Callback>();
	ESaviorResult Result;
	//
	OBJ->Outer = Context;
	OBJ->BeganSave = BeganSaveCallback;
	OBJ->FinishedSave = FinishedSaveCallback;
	//
	OBJ->Target = USavior3::NewSlotInstance(Context->GetWorld(),Savior,Result);
	//
	if (OBJ->Target!=nullptr) {
		OBJ->Target->EVENT_OnFinishDataSAVE.AddDynamic(OBJ,&USAVIOR_SaveGameInstance_Callback::Finish);
		OBJ->Target->WriteMetaOnSave = SaveSlotMeta;
		OBJ->Outer = Context;
	}///
	//
	return OBJ;
}

void USAVIOR_SaveGameInstance_Callback::Activate() {
	if (Target==nullptr||!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	//
	Execute();
}

void USAVIOR_SaveGameInstance_Callback::Execute() {
	BeganSave.ExecuteIfBound();
	ESaviorResult Result;
	//
	Target->SaveGameInstance(Outer,Result);
}

void USAVIOR_SaveGameInstance_Callback::Finish(const bool Succeeded) {
	if (Succeeded) {
		OnSuccess.Broadcast();
	} else {
		OnFail.Broadcast();
	}///
	//
	FinishedSave.ExecuteIfBound(Succeeded);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load Game Instance:

USAVIOR_LoadGameInstance* USAVIOR_LoadGameInstance::LoadGameInstance(USavior3* Savior, UObject* Context) {
	USAVIOR_LoadGameInstance* OBJ = NewObject<USAVIOR_LoadGameInstance>();
	ESaviorResult Result;
	//
	OBJ->Target = USavior3::NewSlotInstance(Context->GetWorld(),Savior,Result);
	//
	if (OBJ->Target!=nullptr) {
		OBJ->Target->EVENT_OnFinishDataLOAD.AddDynamic(OBJ,&USAVIOR_LoadGameInstance::Finish);
		OBJ->Outer = Context;
	}///
	//
	return OBJ;
}

void USAVIOR_LoadGameInstance::Activate() {
	if (Target==nullptr||!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	//
	Execute();
}

void USAVIOR_LoadGameInstance::Execute() {
	ESaviorResult Result;
	//
	Target->LoadGameInstance(Outer,Result);
}

void USAVIOR_LoadGameInstance::Finish(const bool Succeeded) {
	if (Succeeded) {
		OnSuccess.Broadcast();
	} else {
		OnFail.Broadcast();
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load Game Instance [+Callbacks]:

USAVIOR_LoadGameInstance_Callback* USAVIOR_LoadGameInstance_Callback::LoadGameInstance(USavior3* Savior, UObject* Context, const FBeginDATA &BeganLoadCallback, const FEndDATA &FinishedLoadCallback) {
	USAVIOR_LoadGameInstance_Callback* OBJ = NewObject<USAVIOR_LoadGameInstance_Callback>();
	ESaviorResult Result;
	//
	OBJ->BeganLoad = BeganLoadCallback;
	OBJ->FinishedLoad = FinishedLoadCallback;
	OBJ->Target = USavior3::NewSlotInstance(Context->GetWorld(),Savior,Result);
	//
	if (OBJ->Target!=nullptr) {
		OBJ->Target->EVENT_OnFinishDataLOAD.AddDynamic(OBJ,&USAVIOR_LoadGameInstance_Callback::Finish);
		OBJ->Outer = Context;
	}///
	//
	return OBJ;
}

void USAVIOR_LoadGameInstance_Callback::Activate() {
	if (Target==nullptr||!Target->IsValidLowLevelFast()) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target Slot."),ELogVerbosity::Error); return;}
	if (Outer==nullptr||Outer->GetWorld()==nullptr) {FFrame::KismetExecutionMessage(TEXT("[SAVIOR]: Invalid Target World."),ELogVerbosity::Error); return;}
	//
	Execute();
}

void USAVIOR_LoadGameInstance_Callback::Execute() {
	BeganLoad.ExecuteIfBound();
	ESaviorResult Result;
	//
	Target->LoadGameInstance(Outer,Result);
}

void USAVIOR_LoadGameInstance_Callback::Finish(const bool Succeeded) {
	if (Succeeded) {
		OnSuccess.Broadcast();
	} else {
		OnFail.Broadcast();
	}///
	//
	FinishedLoad.ExecuteIfBound(Succeeded);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////