//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//			Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SaviorAsync.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Save Game World:

USAVIOR_SaveGameWorld* USAVIOR_SaveGameWorld::SaveGameWorld(USavior2* Savior, UObject* Context, const bool SaveSlotMeta) {
	USAVIOR_SaveGameWorld* OBJ = NewObject<USAVIOR_SaveGameWorld>();
	ESaviorResult Result;
	//
	OBJ->Target = USavior2::NewSlotInstance(Context->GetWorld(),Savior,Result);
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
	//
	//Target->EVENT_OnFinishDataSAVE.RemoveAll(this);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Save Game World [+Callbacks]:

USAVIOR_SaveGameWorld_Callback* USAVIOR_SaveGameWorld_Callback::SaveGameWorld(USavior2* Savior, UObject* Context, const bool SaveSlotMeta, const FBeginDATA &BeganSaveCallback, const FEndDATA &FinishedSaveCallback) {
	USAVIOR_SaveGameWorld_Callback* OBJ = NewObject<USAVIOR_SaveGameWorld_Callback>();
	ESaviorResult Result;
	//
	OBJ->BeganSave = BeganSaveCallback;
	OBJ->FinishedSave = FinishedSaveCallback;
	//
	OBJ->Target = USavior2::NewSlotInstance(Context->GetWorld(),Savior,Result);
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
	//Target->EVENT_OnFinishDataSAVE.RemoveAll(this);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load Game World:

USAVIOR_LoadGameWorld* USAVIOR_LoadGameWorld::LoadGameWorld(USavior2* Savior, UObject* Context) {
	USAVIOR_LoadGameWorld* OBJ = NewObject<USAVIOR_LoadGameWorld>();
	ESaviorResult Result;
	//
	OBJ->Target = USavior2::NewSlotInstance(Context->GetWorld(),Savior,Result);
	//
	if (OBJ->Target!=nullptr) {
		OBJ->Target->EVENT_OnFinishDataLOAD.AddDynamic(OBJ,&USAVIOR_LoadGameWorld::Finish);
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
	Target->LoadGameWorld(Outer,Result);
}

void USAVIOR_LoadGameWorld::Finish(const bool Succeeded) {
	if (Succeeded) {
		OnSuccess.Broadcast();
	} else {
		OnFail.Broadcast();
	}///
	//
	//Target->EVENT_OnFinishDataLOAD.RemoveAll(this);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load Game World [+Callbacks]:

USAVIOR_LoadGameWorld_Callback* USAVIOR_LoadGameWorld_Callback::LoadGameWorld(USavior2* Savior, UObject* Context, const FBeginDATA &BeganLoadCallback, const FEndDATA &FinishedLoadCallback) {
	USAVIOR_LoadGameWorld_Callback* OBJ = NewObject<USAVIOR_LoadGameWorld_Callback>();
	ESaviorResult Result;
	//
	OBJ->BeganLoad = BeganLoadCallback;
	OBJ->FinishedLoad = FinishedLoadCallback;
	OBJ->Target = USavior2::NewSlotInstance(Context->GetWorld(),Savior,Result);
	//
	if (OBJ->Target!=nullptr) {
		OBJ->Target->EVENT_OnFinishDataLOAD.AddDynamic(OBJ,&USAVIOR_LoadGameWorld_Callback::Finish);
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
	Target->LoadGameWorld(Outer,Result);
}

void USAVIOR_LoadGameWorld_Callback::Finish(const bool Succeeded) {
	if (Succeeded) {
		OnSuccess.Broadcast();
	} else {
		OnFail.Broadcast();
	}///
	//
	FinishedLoad.ExecuteIfBound(Succeeded);
	//Target->EVENT_OnFinishDataLOAD.RemoveAll(this);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Save Level:

USAVIOR_SaveLevel* USAVIOR_SaveLevel::SaveLevel(USavior2* Savior, TSoftObjectPtr<UWorld>LevelToSave, UObject* Context, const bool SaveSlotMeta) {
	USAVIOR_SaveLevel* OBJ = NewObject<USAVIOR_SaveLevel>();
	ESaviorResult Result;
	//
	FString Name;
	LevelToSave.ToString().Split(TEXT("."),nullptr,&Name,ESearchCase::IgnoreCase,ESearchDir::FromEnd);
	//
	OBJ->Target = USavior2::NewSlotInstance(Context->GetWorld(),Savior,Result);
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
	//
	//Target->EVENT_OnFinishDataSAVE.RemoveAll(this);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Save Level [+Callbacks]:

USAVIOR_SaveLevel_Callback* USAVIOR_SaveLevel_Callback::SaveLevel(USavior2* Savior, TSoftObjectPtr<UWorld>LevelToSave, UObject* Context, const bool SaveSlotMeta, const FBeginDATA &BeganSaveCallback, const FEndDATA &FinishedSaveCallback) {
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
	OBJ->Target = USavior2::NewSlotInstance(Context->GetWorld(),Savior,Result);
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
	//Target->EVENT_OnFinishDataSAVE.RemoveAll(this);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load Level:

USAVIOR_LoadLevel* USAVIOR_LoadLevel::LoadLevel(USavior2* Savior, TSoftObjectPtr<UWorld>LevelToLoad, UObject* Context) {
	USAVIOR_LoadLevel* OBJ = NewObject<USAVIOR_LoadLevel>();
	ESaviorResult Result;
	//
	FString Name;
	LevelToLoad.ToString().Split(TEXT("."),nullptr,&Name,ESearchCase::IgnoreCase,ESearchDir::FromEnd);
	//
	OBJ->Target = USavior2::NewSlotInstance(Context->GetWorld(),Savior,Result);
	//
	if (OBJ->Target!=nullptr) {
		OBJ->Target->EVENT_OnFinishDataLOAD.AddDynamic(OBJ,&USAVIOR_LoadLevel::Finish);
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
	Target->LoadLevel(Outer,Level,Result);
}

void USAVIOR_LoadLevel::Finish(const bool Succeeded) {
	if (Succeeded) {
		OnSuccess.Broadcast();
	} else {
		OnFail.Broadcast();
	}///
	//
	//Target->EVENT_OnFinishDataLOAD.RemoveAll(this);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load Level [+Callbacks]:

USAVIOR_LoadLevel_Callback* USAVIOR_LoadLevel_Callback::LoadLevel(USavior2* Savior, TSoftObjectPtr<UWorld>LevelToLoad, UObject* Context, const FBeginDATA &BeganLoadCallback, const FEndDATA &FinishedLoadCallback) {
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
	OBJ->Target = USavior2::NewSlotInstance(Context->GetWorld(),Savior,Result);
	//
	if (OBJ->Target!=nullptr) {
		OBJ->Target->EVENT_OnFinishDataLOAD.AddDynamic(OBJ,&USAVIOR_LoadLevel_Callback::Finish);
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
	//Target->EVENT_OnFinishDataLOAD.RemoveAll(this);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Save Game Mode:

USAVIOR_SaveGameMode* USAVIOR_SaveGameMode::SaveGameMode(USavior2* Savior, UObject* Context, const bool SaveSlotMeta) {
	USAVIOR_SaveGameMode* OBJ = NewObject<USAVIOR_SaveGameMode>();
	ESaviorResult Result;
	//
	OBJ->Target = USavior2::NewSlotInstance(Context->GetWorld(),Savior,Result);
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
	//
	//Target->EVENT_OnFinishDataSAVE.RemoveAll(this);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Save Game Mode [+Callbacks]:

USAVIOR_SaveGameMode_Callback* USAVIOR_SaveGameMode_Callback::SaveGameMode(USavior2* Savior, UObject* Context, const bool SaveSlotMeta, const FBeginDATA &BeganSaveCallback, const FEndDATA &FinishedSaveCallback) {
	USAVIOR_SaveGameMode_Callback* OBJ = NewObject<USAVIOR_SaveGameMode_Callback>();
	ESaviorResult Result;
	//
	OBJ->Outer = Context;
	OBJ->BeganSave = BeganSaveCallback;
	OBJ->FinishedSave = FinishedSaveCallback;
	//
	OBJ->Target = USavior2::NewSlotInstance(Context->GetWorld(),Savior,Result);
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
	//Target->EVENT_OnFinishDataSAVE.RemoveAll(this);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load Game Mode:

USAVIOR_LoadGameMode* USAVIOR_LoadGameMode::LoadGameMode(USavior2* Savior, UObject* Context, const bool IgnoreMainPawnTransform) {
	USAVIOR_LoadGameMode* OBJ = NewObject<USAVIOR_LoadGameMode>();
	ESaviorResult Result;
	//
	OBJ->Target = USavior2::NewSlotInstance(Context->GetWorld(),Savior,Result);
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
	//
	//Target->EVENT_OnFinishDataLOAD.RemoveAll(this);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load Game Mode [+Callbacks]:

USAVIOR_LoadGameMode_Callback* USAVIOR_LoadGameMode_Callback::LoadGameMode(USavior2* Savior, UObject* Context, const bool IgnoreMainPawnTransform, const FBeginDATA &BeganLoadCallback, const FEndDATA &FinishedLoadCallback) {
	USAVIOR_LoadGameMode_Callback* OBJ = NewObject<USAVIOR_LoadGameMode_Callback>();
	ESaviorResult Result;
	//
	OBJ->Outer = Context;
	OBJ->BeganLoad = BeganLoadCallback;
	OBJ->FinishedLoad = FinishedLoadCallback;
	//
	OBJ->Target = USavior2::NewSlotInstance(Context->GetWorld(),Savior,Result);
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
	//Target->EVENT_OnFinishDataLOAD.RemoveAll(this);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Save Game Instance:

USAVIOR_SaveGameInstance* USAVIOR_SaveGameInstance::SaveGameInstance(USavior2* Savior, UObject* Context, const bool SaveSlotMeta) {
	USAVIOR_SaveGameInstance* OBJ = NewObject<USAVIOR_SaveGameInstance>();
	ESaviorResult Result;
	//
	OBJ->Target = USavior2::NewSlotInstance(Context->GetWorld(),Savior,Result);
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
	//
	//Target->EVENT_OnFinishDataSAVE.RemoveAll(this);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Save Game Instance [+Callbacks]:

USAVIOR_SaveGameInstance_Callback* USAVIOR_SaveGameInstance_Callback::SaveGameInstance(USavior2* Savior, UObject* Context, const bool SaveSlotMeta, const FBeginDATA &BeganSaveCallback, const FEndDATA &FinishedSaveCallback) {
	USAVIOR_SaveGameInstance_Callback* OBJ = NewObject<USAVIOR_SaveGameInstance_Callback>();
	ESaviorResult Result;
	//
	OBJ->Outer = Context;
	OBJ->BeganSave = BeganSaveCallback;
	OBJ->FinishedSave = FinishedSaveCallback;
	//
	OBJ->Target = USavior2::NewSlotInstance(Context->GetWorld(),Savior,Result);
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
	//Target->EVENT_OnFinishDataSAVE.RemoveAll(this);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load Game Instance:

USAVIOR_LoadGameInstance* USAVIOR_LoadGameInstance::LoadGameInstance(USavior2* Savior, UObject* Context) {
	USAVIOR_LoadGameInstance* OBJ = NewObject<USAVIOR_LoadGameInstance>();
	ESaviorResult Result;
	//
	OBJ->Target = USavior2::NewSlotInstance(Context->GetWorld(),Savior,Result);
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
	//
	//Target->EVENT_OnFinishDataLOAD.RemoveAll(this);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load Game Instance [+Callbacks]:

USAVIOR_LoadGameInstance_Callback* USAVIOR_LoadGameInstance_Callback::LoadGameInstance(USavior2* Savior, UObject* Context, const FBeginDATA &BeganLoadCallback, const FEndDATA &FinishedLoadCallback) {
	USAVIOR_LoadGameInstance_Callback* OBJ = NewObject<USAVIOR_LoadGameInstance_Callback>();
	ESaviorResult Result;
	//
	OBJ->BeganLoad = BeganLoadCallback;
	OBJ->FinishedLoad = FinishedLoadCallback;
	OBJ->Target = USavior2::NewSlotInstance(Context->GetWorld(),Savior,Result);
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
	//Target->EVENT_OnFinishDataLOAD.RemoveAll(this);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////