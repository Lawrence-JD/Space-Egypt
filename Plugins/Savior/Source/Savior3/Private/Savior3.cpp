//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//			Copyright 2020 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Savior3.h"
#include "Savior3_Shared.h"
#include "LoadScreen/HUD_SaveLoadScreen.h"

#include "Runtime/Core/Public/Misc/App.h"
#include "Runtime/Engine/Classes/Engine/Font.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Components/ActorComponent.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

#include "Policies/CondensedJsonPrintPolicy.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.SerializeEntity"),STAT_FSimpleDelegateGraphTask_SerializeEntity,STATGROUP_TaskGraphTasks);
DECLARE_CYCLE_STAT(TEXT("FSimpleDelegateGraphTask.DeserializeEntity"),STAT_FSimpleDelegateGraphTask_DeserializeEntity,STATGROUP_TaskGraphTasks);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EThreadSafety USavior3::LastThreadState = EThreadSafety::IsCurrentlyThreadSafe;
EThreadSafety USavior3::ThreadSafety = EThreadSafety::IsCurrentlyThreadSafe;

FTimerHandle USavior3::TH_LoadScreen;

float USavior3::SS_Progress = 100.f;
float USavior3::SL_Progress = 100.f;
float USavior3::SS_Workload = 0.f;
float USavior3::SL_Workload = 0.f;
float USavior3::SS_Complete = 0.f;
float USavior3::SL_Complete = 0.f;

static void StaticRemoveLoadScreen() {
	AHUD_SaviorUI::StaticRemoveLoadScreen();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Savior Constructors:

USavior3Settings::USavior3Settings() {
	DefaultPlayerName = TEXT("Player One");
	DefaultChapter = TEXT("Chapter 1");
	DefaultLocation = TEXT("???");
	DefaultPlayerLevel = 1;
	DefaultPlayerID = 0;
	//
	RespawnDynamicComponents = true;
	RespawnDynamicActors = true;
	//
	//
	InstanceScope.Add(UAutoInstanced::StaticClass());
	RespawnScope.Add(UActorComponent::StaticClass());
	RespawnScope.Add(AActor::StaticClass());
	//
	//
	LoadConfig();
}

USlotData::USlotData() {
	Complex.Empty();
	Minimal.Empty();
}

USavior3::USavior3() {
	SetFlagsTo(GetFlags()|RF_MarkAsRootSet);
	//
	SlotData = CreateDefaultSubobject<USlotData>(TEXT("SlotData"));
	SlotMeta = CreateDefaultSubobject<USlotMeta>(TEXT("SlotMeta"));
	if (GetOuter()&&GetOuter()->GetWorld()) {World=GetOuter()->GetWorld();}
	//
	//
	SlotThumbnail = FSoftObjectPath(TEXT("/Savior/Icons/ICO_Thumbnail.ICO_Thumbnail"));
	if ((!IsRunningDedicatedServer())&&(!FeedbackFont.HasValidFont())) {
		ConstructorHelpers::FObjectFinder<UFont>FeedFontOBJ(TEXT("/Engine/EngineFonts/Roboto"));
		if (FeedFontOBJ.Succeeded()){FeedbackFont=FSlateFontInfo(FeedFontOBJ.Object,34,FName("Bold"));}
	}///
	//
	//
	const auto &Settings = GetDefault<USavior3Settings>();
	FeedbackLOAD = FText::FromString(TEXT("LOADING..."));
	FeedbackSAVE = FText::FromString(TEXT("SAVING..."));
	LoadScreenMode = ELoadScreenMode::NoLoadScreen;
	RuntimeMode = ERuntimeMode::BackgroundTask;
	ProgressBarTint = FLinearColor::White;
	SplashStretch = EStretch::Fill;
	ProgressBarOnMovie = true;
	LoadScreenTimer = 1.f;
	BackBlurPower = 25.f;
	//
	ComponentScope.Add(UActorComponent::StaticClass());
	///ObjectScope.Add(UObject::StaticClass());
	ActorScope.Add(AActor::StaticClass());
	//
	WriteMetaOnSave = true;
	DeepLogs = false;
	Debug = false;
	//
	//
	if (GetOutermost()->ContainsMap()) {return;}
	if (HasAnyFlags(RF_DefaultSubObject|RF_ArchetypeObject|RF_Transient)) {return;}
	//
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this,&USavior3::StaticLoadGameWorld);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Savior Routines:

UWorld* USavior3::GetWorld() const {
	const auto Owner = GetOuter();
	if (Owner) {return Owner->GetWorld();}
	//
	return World;
}

void USavior3::SetWorld(UWorld* InWorld) {
	World = InWorld;
}

bool USavior3::CheckInstance() const {
	const bool IsInstance = !IsAsset();
	//
	if (!IsInstance) {
		LOG_SV(true,ESeverity::Error,FString::Printf(TEXT("Slot isn't Instanced. Slots can't be referenced directly in runtime, create a new Slot Instance for:  %s"),*GetFullName()));
	} return IsInstance;
}

void USavior3::PostLoad() {
	Super::PostLoad();
}

void USavior3::PostInitProperties() {
	Super::PostInitProperties();
	//
	if (HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject)) {return;}
	//
	ESaviorResult Result;
	LoadSlotMetaData(Result);
}

void USavior3::BeginDestroy() {
	Super::BeginDestroy();
	//
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Savior Serializer Interface:

void USavior3::WriteSlotToFile(const int32 PlayerID, ESaviorResult &Result) {
	if (SlotData==nullptr) {Result=ESaviorResult::Failed; return;}
	//
	FString SlotID = FString();
	FString MetaName = FString();
	FString DataName = FString();
	//
	if (SlotFileName.IsEmpty()) {
		SlotID = FString(FApp::GetProjectName())+TEXT("_")+GetName();
	} else {SlotID=SlotFileName.ToString();}
	//
	MetaName = SlotID+TEXT("___Meta");
	DataName = (SlotID);
	//
	auto Meta = (WriteMetaOnSave) ? UGameplayStatics::SaveGameToSlot(SlotMeta,MetaName,0) : true;
	auto Saved = UGameplayStatics::SaveGameToSlot(SlotData,DataName,PlayerID);
	//
	if (Meta && Saved) {
		Result=ESaviorResult::Success;
		SlotData->Complex.Empty();
		SlotData->Minimal.Empty();
	return;}
	//
	Result = ESaviorResult::Failed;
}

void USavior3::ReadSlotFromFile(const int32 PlayerID, ESaviorResult &Result) {
	FString SlotID = FString();
	FString MetaName = FString();
	FString DataName = FString();
	//
	if (SlotFileName.IsEmpty()) {
		SlotID = FString(FApp::GetProjectName())+TEXT("_")+GetName();
	} else {SlotID=SlotFileName.ToString();}
	//
	MetaName = SlotID+TEXT("___Meta");
	DataName = (SlotID);
	//
	auto Meta = Cast<USlotMeta>(UGameplayStatics::LoadGameFromSlot(MetaName,0));
	auto Slot = Cast<USlotData>(UGameplayStatics::LoadGameFromSlot(DataName,PlayerID));
	//
	if ((Meta!=nullptr)&&(Slot!=nullptr)) {
		Result=ESaviorResult::Success;
		SlotMeta = Meta;
		SlotData = Slot;
	return;}
	//
	Result = ESaviorResult::Failed;
}

void USavior3::DeleteSlotFile(const int32 PlayerID, ESaviorResult &Result) {
	FString SlotID = FString();
	FString MetaName = FString();
	FString DataName = FString();
	//
	if (SlotFileName.IsEmpty()) {
		SlotID = FString(FApp::GetProjectName())+TEXT("_")+GetName();
	} else {SlotID=SlotFileName.ToString();}
	//
	MetaName = SlotID+TEXT("___Meta");
	DataName = (SlotID);
	//
	if (UGameplayStatics::DoesSaveGameExist(MetaName,0)) {
		UGameplayStatics::DeleteGameInSlot(MetaName,0);
	}///
	//
	if (UGameplayStatics::DoesSaveGameExist(DataName,PlayerID)) {
		UGameplayStatics::DeleteGameInSlot(DataName,PlayerID);
		Result=ESaviorResult::Success;
	return;}
	//
	Result = ESaviorResult::Failed;
}

void USavior3::FindSlotFile(ESaviorResult &Result) {
	FString SlotID = FString();
	//
	if (SlotFileName.IsEmpty()) {
		SlotID = FString(FApp::GetProjectName())+TEXT("_")+GetName();
	} else {SlotID=SlotFileName.ToString();}
	//
	if (UGameplayStatics::DoesSaveGameExist(SlotID,0)) {
		Result = ESaviorResult::Success;
	return;}
	//
	Result = ESaviorResult::Failed;
}

void USavior3::SaveSlotMetaData(ESaviorResult &Result) {
	FString MetaName = FString();
	//
	if (SlotFileName.IsEmpty()) {
		MetaName = FString(FApp::GetProjectName())+TEXT("_")+GetName()+TEXT("___Meta");
	} else {
		MetaName = SlotFileName.ToString();
		MetaName = MetaName+TEXT("___Meta");
	}///
	//
	auto Meta = UGameplayStatics::SaveGameToSlot(SlotMeta,MetaName,0);
	if (Meta) {Result=ESaviorResult::Success; return;}
	//
	Result = ESaviorResult::Failed;
}

void USavior3::LoadSlotMetaData(ESaviorResult &Result) {
	FString MetaName = FString();
	//
	if (SlotFileName.IsEmpty()) {
		MetaName = FString(FApp::GetProjectName())+TEXT("_")+GetName()+TEXT("___Meta");
	} else {
		MetaName = SlotFileName.ToString();
		MetaName = MetaName+TEXT("___Meta");
	}///
	//
	if (!UGameplayStatics::DoesSaveGameExist(MetaName,0)) {
		Result = ESaviorResult::Failed;
	return;}
	//
	auto Meta = Cast<USlotMeta>(UGameplayStatics::LoadGameFromSlot(MetaName,0));
	if (Meta!=nullptr) {
		Result = ESaviorResult::Success;
		SlotMeta = Meta;
	return;}
	//
	Result = ESaviorResult::Failed;
}

const ESaviorResult USavior3::PrepareSlotToSave(const UObject* Context) {
	if (((GetSaveProgress()<100.f)&&(GetSavesDone()>0))||((GetLoadProgress()<100.f)&&(GetLoadsDone()>0))) {
		LOG_SV(Debug,ESeverity::Warning,TEXT("Save or Load action already in progress... Cannot begin another Save action. Save aborted!"));
		EVENT_OnFinishDataSAVE.Broadcast(false);
	return ESaviorResult::Failed;}
	//
	if (SlotData==nullptr||SlotMeta==nullptr||Context==nullptr||!Context->IsValidLowLevelFast()||Context->IsPendingKill()||Context->GetWorld()==nullptr) {
		LOG_SV(Debug,ESeverity::Warning,TEXT("Context Object is invalid to start a Save process... Save aborted!"));
		EVENT_OnFinishDataSAVE.Broadcast(false);
	return ESaviorResult::Failed;}
	//
	//
	USavior3::ThreadSafety = EThreadSafety::IsPreparingToSaveOrLoad;
	World = Context->GetWorld();
	//
	double TimeSpan = SlotMeta->PlayTime + FMath::Abs(World->RealTimeSeconds);
	SlotMeta->PlayTime = FTimespan::FromSeconds(TimeSpan).GetHours();
	SlotMeta->SaveLocation = World->GetName();
	SlotMeta->SaveDate = FDateTime::Now();
	//
	//
	if (LoadScreenTrigger!=ELoadScreenTrigger::WhileLoading) {
		switch (RuntimeMode) {
			case ERuntimeMode::SynchronousTask:
				LaunchLoadScreen(EThreadSafety::SynchronousSaving,FeedbackSAVE);
			break;
			case ERuntimeMode::BackgroundTask:
				LaunchLoadScreen(EThreadSafety::AsynchronousSaving,FeedbackSAVE);
			break;
	default: break;}}
	//
	//
	USavior3::SS_Workload = CalculateWorkload();
	USavior3::SS_Progress = 0.f;
	USavior3::SS_Complete = 0.f;
	//
	EVENT_OnBeginDataSAVE.Broadcast();
	OnPreparedToSave();
	//
	if (PauseGameOnLoad) {
		if (auto PC=World->GetFirstPlayerController()){PC->Pause();}
	}///
	//
	return ESaviorResult::Success;
}

const ESaviorResult USavior3::PrepareSlotToLoad(const UObject* Context) {
	if (((GetSaveProgress()<100.f)&&(GetSavesDone()>0))||((GetLoadProgress()<100.f)&&(GetLoadsDone()>0))) {
		LOG_SV(Debug,ESeverity::Warning,TEXT("Save or Load action already in progress... Cannot begin another Load action. Load aborted!"));
		EVENT_OnFinishDataLOAD.Broadcast(false);
	return ESaviorResult::Failed;}
	//
	if (Context==nullptr||!Context->IsValidLowLevelFast()||Context->IsPendingKill()||Context->GetWorld()==nullptr) {
		LOG_SV(Debug,ESeverity::Warning,TEXT("Context Object is invalid to start a Load process... Load aborted!"));
		EVENT_OnFinishDataLOAD.Broadcast(false);
	return ESaviorResult::Failed;}
	//
	USavior3::ThreadSafety = EThreadSafety::IsPreparingToSaveOrLoad;
	World = Context->GetWorld();
	//
	if (LoadScreenTrigger!=ELoadScreenTrigger::WhileSaving) {
		switch (RuntimeMode) {
			case ERuntimeMode::SynchronousTask:
				LaunchLoadScreen(EThreadSafety::SynchronousLoading,FeedbackLOAD);
			break;
			case ERuntimeMode::BackgroundTask:
				LaunchLoadScreen(EThreadSafety::AsynchronousLoading,FeedbackLOAD);
			break;
	default: break;}}
	//
	USavior3::SL_Workload = CalculateWorkload();
	USavior3::SL_Progress = 0.f;
	USavior3::SL_Complete = 0.f;
	//
	EVENT_OnBeginDataLOAD.Broadcast();
	OnPreparedToLoad();
	//
	if (PauseGameOnLoad) {
		if (auto PC=World->GetFirstPlayerController()){PC->Pause();}
	}///
	//
	return ESaviorResult::Success;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Savior Object Serialization Interface:

void USavior3::SaveObject(UObject* Object, ESaviorResult &Result) {
	if ((USavior3::ThreadSafety==EThreadSafety::SynchronousSaving)||(USavior3::ThreadSafety==EThreadSafety::AsynchronousSaving)) {
		USavior3::SS_Complete++; USavior3::SS_Progress = ((USavior3::SS_Complete/USavior3::SS_Workload)*100);
	}///
	//
	if (Object==nullptr) {Result=ESaviorResult::Failed; return;}
	if (SlotData==nullptr) {Result=ESaviorResult::Failed; return;}
	if (Object->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject|RF_BeginDestroyed)) {Result=ESaviorResult::Failed; return;}
	//
	if (Compression==ERecordType::Complex) {
		const auto ObjectID = Reflector::MakeObjectID(Object);
		const auto Record = GenerateRecord_Object(Object);
		SlotData->Complex.Add(ObjectID,Record);
	} else {
		const auto ObjectID = Reflector::MakeObjectID(Object);
		const auto Record = GenerateMinimalRecord_Object(Object);
		SlotData->Minimal.Add(ObjectID,Record);
	}///
	//
	if (IsInGameThread()) {OnObjectSaved(Object);} else {
		FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
			FSimpleDelegateGraphTask::FDelegate::CreateStatic(&OnObjectSaved,Object),
			GET_STATID(STAT_FSimpleDelegateGraphTask_DeserializeEntity),
			nullptr, ENamedThreads::GameThread
		);//
	}///
	//
	Result = ESaviorResult::Success;
}

void USavior3::LoadObject(UObject* Object, ESaviorResult &Result) {
	if ((USavior3::ThreadSafety==EThreadSafety::SynchronousLoading)||(USavior3::ThreadSafety==EThreadSafety::AsynchronousLoading)) {
		USavior3::SL_Complete++; USavior3::SL_Progress = ((USavior3::SL_Complete/USavior3::SL_Workload)*100);
	}///
	//
	if (Object==nullptr) {Result=ESaviorResult::Failed; return;}
	if (SlotData==nullptr) {Result=ESaviorResult::Failed; return;}
	if (Object->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject|RF_BeginDestroyed)) {Result=ESaviorResult::Failed; return;}
	//
	const auto ObjectID = Reflector::MakeObjectID(Object);
	//
	if (Compression==ERecordType::Complex) {
		if (!SlotData->Complex.Contains(ObjectID)) {Result=ESaviorResult::Failed; return;}
	} else {
		if (!SlotData->Minimal.Contains(ObjectID)) {Result=ESaviorResult::Failed; return;}
	}///
	//
	if (Compression==ERecordType::Complex) {
		const auto Record = SlotData->Complex.FindRef(ObjectID);
		//
		if (Record.Destroyed) {
			MarkObjectAutoDestroyed(Object);
			Result = ESaviorResult::Success;
		} else {UnpackRecord_Object(Record,Object,Result);}
	} else {
		const auto Record = SlotData->Minimal.FindRef(ObjectID);
		//
		if (Record.Destroyed) {
			MarkObjectAutoDestroyed(Object);
			Result = ESaviorResult::Success;
		} else {UnpackMinimalRecord_Object(Record,Object,Result);}
	}///
	//
	if (IsInGameThread()) {OnObjectLoaded(FSlotMetaData(SlotMeta),Object);} else {
		FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
			FSimpleDelegateGraphTask::FDelegate::CreateStatic(&OnObjectLoaded,FSlotMetaData(SlotMeta),Object),
			GET_STATID(STAT_FSimpleDelegateGraphTask_DeserializeEntity),
			nullptr, ENamedThreads::GameThread
		);//
	}///
}

void USavior3::SaveObjectHierarchy(UObject* Object, ESaviorResult &Result) {
	if (Object==nullptr||GetWorld()==nullptr) {Result=ESaviorResult::Failed; return;}
	if (Object->IsA(UActorComponent::StaticClass())||Object->IsA(UActorComponent::StaticClass())) {
		LOG_SV(Debug,ESeverity::Warning,TEXT("SaveObjectHierarchy() is not usable on Actors. Use 'Save Actor Hierarchy' function instead."));
		Result = ESaviorResult::Failed; return;
	}///
	//
	SaveObject(Object,Result);
	if (Result==ESaviorResult::Failed) {return;}
	//
	TArray<UObject*>Children;
	ForEachObjectWithOuter(Object,[&Children](UObject*Chid){Children.Add(Chid);},true);
	//
	for (const auto &OBJ : Children) {
		LOG_SV(Debug,ESeverity::Warning,OBJ->GetName());
		SaveObject(OBJ,Result);
	}///
}

void USavior3::LoadObjectHierarchy(UObject* Object, ESaviorResult &Result) {
	if (Object==nullptr||GetWorld()==nullptr) {Result=ESaviorResult::Failed; return;}
	if (Object->IsA(UActorComponent::StaticClass())||Object->IsA(UActorComponent::StaticClass())) {
		LOG_SV(Debug,ESeverity::Warning,TEXT("LoadObjectHierarchy() is not usable on Actors. Use 'Save Actor Hierarchy' function instead."));
		Result = ESaviorResult::Failed; return;
	}///
	//
	LoadObject(Object,Result);
	if (Result==ESaviorResult::Failed) {return;}
	//
	TArray<UObject*>Children;
	ForEachObjectWithOuter(Object,[&Children](UObject*Chid){Children.Add(Chid);},true);
	//
	for (const auto &OBJ : Children) {
		LOG_SV(Debug,ESeverity::Warning,OBJ->GetName());
		LoadObject(OBJ,Result);
	}///
}

void USavior3::SaveObjectsOfClass(TSubclassOf<UObject>Class, const bool SaveHierarchy, ESaviorResult &Result) {
	if (Class.Get()==nullptr) {Result=ESaviorResult::Failed; return;}
	//
	if (Class.Get()->IsChildOf(UActorComponent::StaticClass())||Class.Get()->IsChildOf(UActorComponent::StaticClass())) {
		LOG_SV(Debug,ESeverity::Warning,TEXT("SaveObjectsOfClass() is not usable on Actors. Use 'Save Actor Hierarchy' function instead."));
		Result = ESaviorResult::Failed; return;
	}///
	//
	for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
		if (!OBJ->IsValidLowLevelFast()) {continue;}
		if (!OBJ->IsA(Class.Get())) {continue;}
		//
		if ((*OBJ)==GetWorld()) {continue;}
		if (!OBJ->GetOutermost()->ContainsMap()) {continue;}
		if (OBJ->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject|RF_BeginDestroyed)) {continue;}
		//
		if (!SaveHierarchy) {SaveObject(*OBJ,Result);}
		else {SaveObjectHierarchy(*OBJ,Result);}
	}///
}

void USavior3::LoadObjectsOfClass(TSubclassOf<UObject>Class, const bool LoadHierarchy, ESaviorResult &Result) {
	if (Class.Get()==nullptr) {Result=ESaviorResult::Failed; return;}
	//
	if (Class.Get()->IsChildOf(UActorComponent::StaticClass())||Class.Get()->IsChildOf(UActorComponent::StaticClass())) {
		LOG_SV(Debug,ESeverity::Warning,TEXT("LoadObjectsOfClass() is not usable on Actors. Use 'Save Actor Hierarchy' function instead."));
		Result = ESaviorResult::Failed; return;
	}///
	//
	for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
		if (!OBJ->IsValidLowLevelFast()) {continue;}
		if (!OBJ->IsA(Class.Get())) {continue;}
		//
		if ((*OBJ)==GetWorld()) {continue;}
		if (!OBJ->GetOutermost()->ContainsMap()) {continue;}
		if (OBJ->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject|RF_BeginDestroyed)) {continue;}
		//
		if (!LoadHierarchy) {LoadObject(*OBJ,Result);}
		else {LoadObjectHierarchy(*OBJ,Result);}
	}///
}

void USavior3::SaveComponent(UObject* Context, UActorComponent* Component, ESaviorResult &Result) {
	if ((USavior3::ThreadSafety==EThreadSafety::SynchronousSaving)||(USavior3::ThreadSafety==EThreadSafety::AsynchronousSaving)) {
		USavior3::SS_Complete++; USavior3::SS_Progress = ((USavior3::SS_Complete/USavior3::SS_Workload)*100);
	}///
	//
	if (Context->GetWorld()==nullptr) {Result=ESaviorResult::Failed; return;}
	//
	World = Context->GetWorld();
	//
	if (SlotData==nullptr) {Result=ESaviorResult::Failed; return;}
	if (Component==nullptr) {Result=ESaviorResult::Failed; return;}
	if (TAG_CNOSAVE(Component)) {Result=ESaviorResult::Failed; return;}
	if (Component->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject|RF_BeginDestroyed)) {Result=ESaviorResult::Failed; return;}
	//
	if (Compression==ERecordType::Complex) {
		const auto ComponentID = Reflector::MakeComponentID(Component,true);
		const auto Record = GenerateRecord_Component(Component);
		SlotData->Complex.Add(ComponentID,Record);
	} else {
		const auto ComponentID = Reflector::MakeComponentID(Component);
		const auto Record = GenerateMinimalRecord_Component(Component);
		SlotData->Minimal.Add(ComponentID,Record);
	}///
	//
	if (IsInGameThread()) {OnComponentSaved(Component);} else {
		FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
			FSimpleDelegateGraphTask::FDelegate::CreateStatic(&OnComponentSaved,Component),
			GET_STATID(STAT_FSimpleDelegateGraphTask_SerializeEntity),
			nullptr, ENamedThreads::GameThread
		);//
	}///
	//
	Result = ESaviorResult::Success;
}

void USavior3::LoadComponent(UObject* Context, UActorComponent* Component, ESaviorResult &Result) {
	if ((USavior3::ThreadSafety==EThreadSafety::SynchronousLoading)||(USavior3::ThreadSafety==EThreadSafety::AsynchronousLoading)) {
		USavior3::SL_Complete++; USavior3::SL_Progress = ((USavior3::SL_Complete/USavior3::SL_Workload)*100);
	}///
	//
	if (Context->GetWorld()==nullptr) {Result=ESaviorResult::Failed; return;}
	//
	World = Context->GetWorld();
	//
	if (SlotData==nullptr) {Result=ESaviorResult::Failed; return;}
	if (Component==nullptr) {Result=ESaviorResult::Failed; return;}
	if (TAG_CNOLOAD(Component)) {Result=ESaviorResult::Failed; return;}
	if (Component->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject|RF_BeginDestroyed)) {Result=ESaviorResult::Failed; return;}
	//
	FName ComponentID = NAME_None;
	if (Compression==ERecordType::Complex) {
		ComponentID = Reflector::MakeComponentID(Component,true);
		if (!SlotData->Complex.Contains(ComponentID)) {Result=ESaviorResult::Failed; return;}
	} else {
		ComponentID = Reflector::MakeComponentID(Component);
		if (!SlotData->Minimal.Contains(ComponentID)) {Result=ESaviorResult::Failed; return;}
	}///
	//
	if (Compression==ERecordType::Complex) {
		const auto Record = SlotData->Complex.FindRef(ComponentID);
		if (!TAG_CNOKILL(Component)&&(Record.Destroyed)) {
			MarkComponentAutoDestroyed(Component);
			Result = ESaviorResult::Success;
		} else {UnpackRecord_Component(Record,Component,Result);}
	} else {
		const auto Record = SlotData->Minimal.FindRef(ComponentID);
		if (!TAG_CNOKILL(Component)&&(Record.Destroyed)) {
			MarkComponentAutoDestroyed(Component);
			Result = ESaviorResult::Success;
		} else {UnpackMinimalRecord_Component(Record,Component,Result);}
	}///
	//
	if (IsInGameThread()) {OnComponentLoaded(FSlotMetaData(SlotMeta),Component);} else {
		FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
			FSimpleDelegateGraphTask::FDelegate::CreateStatic(&OnComponentLoaded,FSlotMetaData(SlotMeta),Component),
			GET_STATID(STAT_FSimpleDelegateGraphTask_DeserializeEntity),
			nullptr, ENamedThreads::GameThread
		);//
	}///
}

void USavior3::LoadComponentWithGUID(UObject* Context, const FGuid &SGUID, ESaviorResult &Result) {
	if (!SGUID.IsValid()) {Result=ESaviorResult::Failed; return;}
	//
	if (Context->GetWorld()==nullptr) {Result=ESaviorResult::Failed; return;}
	World = Context->GetWorld();
	//
	if (World==nullptr) {Result=ESaviorResult::Failed; return;}
	if (SlotData==nullptr) {Result=ESaviorResult::Failed; return;}
	//
	UActorComponent* Component = nullptr;
	for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
		if (!OBJ->IsA(UActorComponent::StaticClass())) {continue;}
		//
		if ((*OBJ)==World) {continue;}
		if (OBJ->GetWorld()!=World) {continue;}
		if (!OBJ->GetOutermost()->ContainsMap()) {continue;}
		if (OBJ->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject|RF_BeginDestroyed)) {continue;}
		//
		const FGuid UID = Reflector::FindGUID(*OBJ);
		if (UID==SGUID) {Component=(CastChecked<UActorComponent>(*OBJ)); break;}
	}///
	//
	if (Component==nullptr) {
		Result = ESaviorResult::Failed;
		LOG_SV(true,ESeverity::Warning,FString::Printf(TEXT("[Load Component by GUID]: Couldn't find in World any Component owner of this SGUID:  %s"),*SGUID.ToString()));
	return;}
	//
	if (Compression==ERecordType::Complex) {
		for (auto IT = SlotData->Complex.CreateConstIterator(); IT; ++IT) {
			const FSaviorRecord &Record = IT->Value;
			//
			if (Record.GUID==SGUID) {
				if (TAG_CNOLOAD(Component)) {Result=ESaviorResult::Failed; return;}
				if (!TAG_CNOKILL(Component)&&(Record.Destroyed)) {
					Result = ESaviorResult::Success;
					MarkComponentAutoDestroyed(Component);
				} else {UnpackRecord_Component(Record,Component,Result);}
			}///
		}///
	} else {Result=ESaviorResult::Failed; return;}
	//
	if (IsInGameThread()) {OnComponentLoaded(FSlotMetaData(SlotMeta),Component);} else {
		FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
			FSimpleDelegateGraphTask::FDelegate::CreateStatic(&OnComponentLoaded,FSlotMetaData(SlotMeta),Component),
			GET_STATID(STAT_FSimpleDelegateGraphTask_DeserializeEntity),
			nullptr, ENamedThreads::GameThread
		);//
	}///
}

void USavior3::SaveAnimation(UObject* Context, ACharacter* Character, ESaviorResult &Result) {
	if (Context->GetWorld()==nullptr) {Result=ESaviorResult::Failed; return;}
	//
	if (const auto &Skel=Character->GetMesh()) {
		if (const auto &Anim=Skel->GetAnimInstance()) {
			const auto &Interface = Cast<ISAVIOR_Serializable>(Anim);
			if (Interface||Anim->GetClass()->ImplementsInterface(USAVIOR_Serializable::StaticClass())) {
				const auto ActorAnimID = Reflector::MakeObjectID(Anim);
				const auto AnimRecord = GenerateRecord_Object(Anim);
				//
				SlotData->Complex.Add(ActorAnimID,AnimRecord);
				if (IsInGameThread()) {OnAnimationSaved(Anim);} else {
					FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
						FSimpleDelegateGraphTask::FDelegate::CreateStatic(&OnAnimationSaved,Anim),
						GET_STATID(STAT_FSimpleDelegateGraphTask_SerializeEntity),
						nullptr, ENamedThreads::GameThread
					);//
				}///
			} else {Result=ESaviorResult::Failed; return;}
		} else {Result=ESaviorResult::Failed; return;}
	} else {Result=ESaviorResult::Failed; return;}
	//
	Result = ESaviorResult::Success;
}

void USavior3::LoadAnimation(UObject* Context, ACharacter* Character, ESaviorResult &Result) {
	if (Context->GetWorld()==nullptr) {Result=ESaviorResult::Failed; return;}
	//
	if (const auto &Skel=Character->GetMesh()) {
		if (const auto &Anim=Skel->GetAnimInstance()) {
			const auto &Interface = Cast<ISAVIOR_Serializable>(Anim);
			if (Interface||Anim->GetClass()->ImplementsInterface(USAVIOR_Serializable::StaticClass())) {
				const auto ActorAnimID = Reflector::MakeObjectID(Anim);
				//
				if (!SlotData->Complex.Contains(ActorAnimID)) {Result=ESaviorResult::Failed; return;}
				const auto AnimRecord = SlotData->Complex.FindRef(ActorAnimID);
				UnpackRecord_Object(AnimRecord,Anim,Result);
				//
				if (IsInGameThread()) {OnAnimationLoaded(FSlotMetaData(SlotMeta),Anim);} else {
					FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
						FSimpleDelegateGraphTask::FDelegate::CreateStatic(&OnAnimationLoaded,FSlotMetaData(SlotMeta),Anim),
						GET_STATID(STAT_FSimpleDelegateGraphTask_DeserializeEntity),
						nullptr, ENamedThreads::GameThread
					);//
				}///
			} else {Result=ESaviorResult::Failed; return;}
		} else {Result=ESaviorResult::Failed; return;}
	} else {Result=ESaviorResult::Failed; return;}
	//
	Result = ESaviorResult::Success;
}

void USavior3::SaveActorMaterials(UObject* Context, AActor* Actor, ESaviorResult &Result) {
	if (Actor->GetWorld()==nullptr||Actor->IsPendingKill()||!Actor->GetWorld()->IsGameWorld()) {Result=ESaviorResult::Failed; return;}
	if (Context==nullptr||!Context->IsValidLowLevel()) {Result=ESaviorResult::Failed; return;}
	//
	const auto &Interface = Cast<ISAVIOR_Serializable>(Actor);
	if ((Interface==nullptr)&&(!Actor->GetClass()->ImplementsInterface(USAVIOR_Serializable::StaticClass()))) {
		Result=ESaviorResult::Failed; return;
	}///
	//
	const auto ActorID = Reflector::MakeActorID(Actor);
	FSaviorRecord &ActorRecord = SlotData->Complex.FindOrAdd(ActorID);
	//
	TArray<UStaticMeshComponent*>Meshes;
	TArray<USkeletalMeshComponent*>Skins;
	//
	Actor->GetComponents<UStaticMeshComponent>(Meshes);
	Actor->GetComponents<USkeletalMeshComponent>(Skins);
	//
	for (auto CMP : Meshes) {
		const auto Mesh = CastChecked<UStaticMeshComponent>(CMP);
		TArray<UMaterialInterface*>Mats = Mesh->GetMaterials();
		//
		for (auto Mat : Mats) {
			if (!Mat->IsValidLowLevel()) {continue;}
			if (!Mat->IsA(UMaterialInstanceDynamic::StaticClass())) {continue;}
			//
			FMaterialRecord MatInfo = CaptureMaterialSnapshot(CastChecked<UMaterialInstanceDynamic>(Mat));
			const FName MatID = *FString::Printf(TEXT("%s.%s"),*Actor->GetName(),*Mat->GetName());
			ActorRecord.Materials.Add(MatID,MatInfo);
		}///
	}///
	//
	for (auto CMP : Skins) {
		const auto Mesh = CastChecked<USkeletalMeshComponent>(CMP);
		TArray<UMaterialInterface*>Mats = Mesh->GetMaterials();
		//
		for (auto Mat : Mats) {
			if (!Mat->IsValidLowLevel()) {continue;}
			if (!Mat->IsA(UMaterialInstanceDynamic::StaticClass())) {continue;}
			//
			FMaterialRecord MatInfo = CaptureMaterialSnapshot(CastChecked<UMaterialInstanceDynamic>(Mat));
			const FName MatID = *FString::Printf(TEXT("%s.%s"),*Actor->GetName(),*Mat->GetName());
			ActorRecord.Materials.Add(MatID,MatInfo);
		}///
	}///
}

void USavior3::LoadActorMaterials(UObject* Context, AActor* Actor, ESaviorResult &Result) {
	if (Actor->GetWorld()==nullptr||Actor->IsPendingKill()||!Actor->GetWorld()->IsGameWorld()) {Result=ESaviorResult::Failed; return;}
	if (Context==nullptr||!Context->IsValidLowLevel()) {Result=ESaviorResult::Failed; return;}
	//
	const auto &Interface = Cast<ISAVIOR_Serializable>(Actor);
	if ((Interface==nullptr)&&(!Actor->GetClass()->ImplementsInterface(USAVIOR_Serializable::StaticClass()))) {
		Result=ESaviorResult::Failed; return;
	}///
	//
	const auto ActorID = Reflector::MakeActorID(Actor);
	if (!SlotData->Complex.Contains(ActorID)) {Result=ESaviorResult::Failed; return;}
	const auto ActorRecord = SlotData->Complex.FindRef(ActorID);
	//
	TArray<UStaticMeshComponent*>Meshes;
	TArray<USkeletalMeshComponent*>Skins;
	//
	Actor->GetComponents<UStaticMeshComponent>(Meshes);
	Actor->GetComponents<USkeletalMeshComponent>(Skins);
	//
	for (auto CMP : Meshes) {
		const auto Mesh = CastChecked<UStaticMeshComponent>(CMP);
		TArray<UMaterialInterface*>Mats = Mesh->GetMaterials();
		//
		for (auto Mat : Mats) {
			if (!Mat->IsValidLowLevel()) {continue;}
			if (!Mat->IsA(UMaterialInstanceDynamic::StaticClass())) {continue;}
			RestoreMaterialFromSnapshot(CastChecked<UMaterialInstanceDynamic>(Mat),ActorRecord);
		}///
	}///
	//
	for (auto CMP : Skins) {
		const auto Mesh = CastChecked<USkeletalMeshComponent>(CMP);
		TArray<UMaterialInterface*>Mats = Mesh->GetMaterials();
		//
		for (auto Mat : Mats) {
			if (!Mat->IsValidLowLevel()) {continue;}
			if (!Mat->IsA(UMaterialInstanceDynamic::StaticClass())) {continue;}
			RestoreMaterialFromSnapshot(CastChecked<UMaterialInstanceDynamic>(Mat),ActorRecord);
		}///
	}///
}

void USavior3::SaveActor(UObject* Context, AActor* Actor, ESaviorResult &Result) {
	if ((USavior3::ThreadSafety==EThreadSafety::SynchronousSaving)||(USavior3::ThreadSafety==EThreadSafety::AsynchronousSaving)) {
		USavior3::SS_Complete++; USavior3::SS_Progress = ((USavior3::SS_Complete/USavior3::SS_Workload)*100);
	}///
	//
	if (Context->GetWorld()==nullptr) {Result=ESaviorResult::Failed; return;}
	World = Context->GetWorld();
	//
	if (Actor==nullptr) {Result=ESaviorResult::Failed; return;}
	if (SlotData==nullptr) {Result=ESaviorResult::Failed; return;}
	if (TAG_ANOSAVE(Actor)) {Result=ESaviorResult::Failed; return;}
	if (Actor->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject|RF_BeginDestroyed)) {Result=ESaviorResult::Failed; return;}
	//
	if (Compression==ERecordType::Complex) {
		const auto ActorID = Reflector::MakeActorID(Actor,true);
		const auto Record = GenerateRecord_Actor(Actor);
		SlotData->Complex.Add(ActorID,Record);
		//
		if (IsInGameThread()) {SerializeActorMaterials(this,Actor);} else {
			FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
				FSimpleDelegateGraphTask::FDelegate::CreateStatic(&SerializeActorMaterials,this,Actor),
				GET_STATID(STAT_FSimpleDelegateGraphTask_SerializeEntity),
				nullptr, ENamedThreads::GameThread
			);//
		}///
		//
		if (Actor->IsA(ACharacter::StaticClass())) {
			SaveAnimation(Context,CastChecked<ACharacter>(Actor),Result);
		}///
	} else {
		const auto ActorID = Reflector::MakeActorID(Actor);
		const auto Record = GenerateMinimalRecord_Actor(Actor);
		SlotData->Minimal.Add(ActorID,Record);
	}///
	//
	if (IsInGameThread()) {OnActorSaved(Actor);} else {
		FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
			FSimpleDelegateGraphTask::FDelegate::CreateStatic(&OnActorSaved,Actor),
			GET_STATID(STAT_FSimpleDelegateGraphTask_SerializeEntity),
			nullptr, ENamedThreads::GameThread
		);//
	}///
	//
	Result = ESaviorResult::Success;
}

void USavior3::LoadActor(UObject* Context, AActor* Actor, ESaviorResult &Result) {
	if ((USavior3::ThreadSafety==EThreadSafety::SynchronousLoading)||(USavior3::ThreadSafety==EThreadSafety::AsynchronousLoading)) {
		USavior3::SL_Complete++; USavior3::SL_Progress = ((USavior3::SL_Complete/USavior3::SL_Workload)*100);
	}///
	//
	if (Context->GetWorld()==nullptr) {Result=ESaviorResult::Failed; return;}
	World = Context->GetWorld();
	//
	if (Actor==nullptr) {Result=ESaviorResult::Failed; return;}
	if (SlotData==nullptr) {Result=ESaviorResult::Failed; return;}
	if (TAG_ANOLOAD(Actor)) {Result=ESaviorResult::Failed; return;}
	if (Actor->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject|RF_BeginDestroyed)) {Result=ESaviorResult::Failed; return;}
	//
	//
	FName ActorID = NAME_None;
	if (Compression==ERecordType::Complex) {
		ActorID = Reflector::MakeActorID(Actor,true);
		if (!SlotData->Complex.Contains(ActorID)) {Result=ESaviorResult::Failed; return;}
	} else {
		ActorID = Reflector::MakeActorID(Actor);
		if (!SlotData->Minimal.Contains(ActorID)) {Result=ESaviorResult::Failed; return;}
	}///
	//
	if (Compression==ERecordType::Complex) {
		auto Record = SlotData->Complex.FindRef(ActorID);
		if (Actor->IsA(APawn::StaticClass())) {Record.IgnoreTransform=IgnorePawnTransformOnLoad;}
		//
		if (!TAG_ANOKILL(Actor)&&(Record.Destroyed)) {
			MarkActorAutoDestroyed(Actor);
			Result = ESaviorResult::Success;
		} else {
			UnpackRecord_Actor(Record,Actor,Result);
			//
			if (IsInGameThread()) {DeserializeActorMaterials(this,Actor);} else {
				FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
					FSimpleDelegateGraphTask::FDelegate::CreateStatic(&DeserializeActorMaterials,this,Actor),
					GET_STATID(STAT_FSimpleDelegateGraphTask_SerializeEntity),
					nullptr, ENamedThreads::GameThread
				);//
			}///
			//
			if (Actor->IsA(ACharacter::StaticClass())) {
				LoadAnimation(Context,CastChecked<ACharacter>(Actor),Result);
			}///
		}///
	} else {
		const auto Record = SlotData->Minimal.FindRef(ActorID);
		if (!TAG_ANOKILL(Actor)&&(Record.Destroyed)) {
			MarkActorAutoDestroyed(Actor);
			Result = ESaviorResult::Success;
		} else {UnpackMinimalRecord_Actor(Record,Actor,Result);}
	}///
	//
	if (IsInGameThread()) {OnActorLoaded(FSlotMetaData(SlotMeta),Actor);} else {
		FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
			FSimpleDelegateGraphTask::FDelegate::CreateStatic(&OnActorLoaded,FSlotMetaData(SlotMeta),Actor),
			GET_STATID(STAT_FSimpleDelegateGraphTask_DeserializeEntity),
			nullptr, ENamedThreads::GameThread
		);//
	}///
}

AActor* USavior3::LoadActorWithGUID(UObject* Context, const FGuid &SGUID, ESaviorResult &Result) {
	if (!SGUID.IsValid()) {Result=ESaviorResult::Failed; return nullptr;}
	//
	if (Context->GetWorld()==nullptr) {Result=ESaviorResult::Failed; return nullptr;}
	World = Context->GetWorld();
	//
	if (World==nullptr) {Result=ESaviorResult::Failed; return nullptr;}
	if (SlotData==nullptr) {Result=ESaviorResult::Failed; return nullptr;}
	//
	AActor* Actor = nullptr;
	for (TActorIterator<AActor>ACT(World); ACT; ++ACT) {
		if (!(*ACT)->GetOutermost()->ContainsMap()) {continue;}
		if (!(*ACT)->IsValidLowLevelFast()||(*ACT)->IsPendingKill()) {continue;}
		if ((*ACT)->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject|RF_BeginDestroyed)) {continue;}
		//
		if (TAG_ANOLOAD(*ACT)) {continue;}
		//
		const FGuid UID = Reflector::FindGUID(*ACT);
		if (UID==SGUID) {Actor=(*ACT); break;}
	}///
	//
	if (Actor==nullptr) {
		Result = ESaviorResult::Failed;
		LOG_SV(true,ESeverity::Warning,FString::Printf(TEXT("[Load Actor by GUID]: Couldn't find in World any Actor owner of this SGUID:  %s"),*SGUID.ToString()));
	return Actor;}
	//
	if (Compression==ERecordType::Complex) {
		for (auto IT = SlotData->Complex.CreateConstIterator(); IT; ++IT) {
			const FSaviorRecord &Record = IT->Value;
			//
			if (Record.GUID==SGUID) {
				if (!TAG_ANOKILL(Actor)&&(Record.Destroyed)) {
					MarkActorAutoDestroyed(Actor);
					Result = ESaviorResult::Success;
				} else {UnpackRecord_Actor(Record,Actor,Result);}
			}///
		}///
	} else {Result=ESaviorResult::Failed; return Actor;}
	//
	if (IsInGameThread()) {OnActorLoaded(FSlotMetaData(SlotMeta),Actor);} else {
		FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
			FSimpleDelegateGraphTask::FDelegate::CreateStatic(&OnActorLoaded,FSlotMetaData(SlotMeta),Actor),
			GET_STATID(STAT_FSimpleDelegateGraphTask_DeserializeEntity),
			nullptr, ENamedThreads::GameThread
		);//
	}///
	//
	return Actor;
}

void USavior3::SaveActorHierarchy(UObject* Context, AActor* Actor, ESaviorResult &Result) {
	if (Actor==nullptr) {Result=ESaviorResult::Failed; return;}
	//
	if (Context->GetWorld()==nullptr) {Result=ESaviorResult::Failed; return;}
	World = Context->GetWorld();
	//
	SaveActor(World,Actor,Result);
	if (Result==ESaviorResult::Failed) {return;}
	//
	TArray<AActor*>Children;
	TArray<AActor*>Attached;
	TSet<UActorComponent*>Components;
	//
	Actor->GetAllChildActors(Children);
	Actor->GetAttachedActors(Attached);
	Components = Actor->GetComponents();
	//
	for (auto CMP : Components.Array()) {SaveComponent(World,CMP,Result);}
	for (auto Sub : Attached) {SaveActor(World,Sub,Result); Components = Sub->GetComponents(); for (auto CMP : Components.Array()) {SaveComponent(World,CMP,Result);}}
	for (auto Child : Children) {SaveActor(World,Child,Result); Components = Child->GetComponents(); for (auto CMP : Components.Array()) {SaveComponent(World,CMP,Result);}}
}

void USavior3::LoadActorHierarchy(UObject* Context, AActor* Actor, ESaviorResult &Result) {
	if (Actor==nullptr) {Result=ESaviorResult::Failed; return;}
	//
	if (Context->GetWorld()==nullptr) {Result=ESaviorResult::Failed; return;}
	World = Context->GetWorld();
	//
	LoadActor(World,Actor,Result);
	if (Result==ESaviorResult::Failed) {return;}
	//
	TArray<AActor*>Children;
	TArray<AActor*>Attached;
	TSet<UActorComponent*>Components;
	//
	Actor->GetAllChildActors(Children);
	Actor->GetAttachedActors(Attached);
	Components = Actor->GetComponents();
	//
	for (auto CMP : Components.Array()) {LoadComponent(World,CMP,Result);}
	for (auto Sub : Attached) {LoadActor(World,Sub,Result); Components = Sub->GetComponents(); for (auto CMP : Components.Array()) {LoadComponent(World,CMP,Result);}}
	for (auto Child : Children) {LoadActor(World,Child,Result); Components = Child->GetComponents(); for (auto CMP : Components.Array()) {LoadComponent(World,CMP,Result);}}
}

void USavior3::LoadActorHierarchyWithGUID(UObject* Context, const FGuid &SGUID, ESaviorResult &Result) {
	if (!SGUID.IsValid()) {Result=ESaviorResult::Failed; return;}
	//
	if (Context->GetWorld()==nullptr) {Result=ESaviorResult::Failed; return;}
	World = Context->GetWorld();
	//
	AActor* Actor = LoadActorWithGUID(World,SGUID,Result);
	if (Result==ESaviorResult::Failed) {return;}
	//
	TArray<AActor*>Children;
	TArray<AActor*>Attached;
	TSet<UActorComponent*>Components;
	//
	Actor->GetAllChildActors(Children);
	Actor->GetAttachedActors(Attached);
	Components = Actor->GetComponents();
	//
	for (auto CMP : Components.Array()) {
		const FGuid UID = Reflector::FindGUID(CMP);
		if (!UID.IsValid()) {continue;}
		//
		LoadComponentWithGUID(World,UID,Result);
	}///
	//
	for (auto Sub : Attached) {
		const FGuid UID = Reflector::FindGUID(Sub);
		if (!UID.IsValid()) {continue;}
		//
		for (auto IT = SlotData->Complex.CreateConstIterator(); IT; ++IT) {
			const FSaviorRecord &Record = IT->Value;
			//
			if (Record.GUID==UID) {
				LoadActorHierarchyWithGUID(World,UID,Result);
			break;}
		}///
		//
		TSet<UActorComponent*> SubCMP;
		SubCMP = Sub->GetComponents();
		//
		for (auto CMP : SubCMP.Array()) {
			const FGuid CUID = Reflector::FindGUID(CMP);
			if (!CUID.IsValid()) {continue;}
			//
			LoadComponentWithGUID(World,CUID,Result);
		}///
	}///
	//
	for (auto Child : Children) {
		const FGuid UID = Reflector::FindGUID(Child);
		if (!UID.IsValid()) {continue;}
		//
		for (auto IT = SlotData->Complex.CreateConstIterator(); IT; ++IT) {
			const FSaviorRecord &Record = IT->Value;
			//
			if (Record.GUID==UID) {
				LoadActorHierarchyWithGUID(World,UID,Result);
			break;}
		}///
		//
		TSet<UActorComponent*> SubCMP;
		SubCMP = Child->GetComponents();
		//
		for (auto CMP : SubCMP.Array()) {
			const FGuid CUID = Reflector::FindGUID(CMP);
			if (!CUID.IsValid()) {continue;}
			//
			LoadComponentWithGUID(World,CUID,Result);
		}///
	}///
}

void USavior3::SaveActorsOfClass(UObject* Context, TSubclassOf<AActor>Class, const bool SaveHierarchy, ESaviorResult &Result) {
	if (Class.Get()==nullptr) {Result=ESaviorResult::Failed; return;}
	//
	if (Context->GetWorld()==nullptr) {
		Result = ESaviorResult::Failed;
	return;} World = Context->GetWorld();
	//
	for (TActorIterator<AActor>Actor(World); Actor; ++Actor) {
		if ((*Actor)->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject|RF_BeginDestroyed)) {continue;}
		if (!Actor->IsA(Class.Get())) {continue;}
		//
		if (!SaveHierarchy) {SaveActor(World,*Actor,Result);}
		else {SaveActorHierarchy(World,*Actor,Result);}
	}///
}

void USavior3::LoadActorsOfClass(UObject* Context, TSubclassOf<AActor>Class, const bool LoadHierarchy, ESaviorResult &Result) {
	if (Class.Get()==nullptr) {Result=ESaviorResult::Failed; return;}
	//
	if (Context->GetWorld()==nullptr) {
		Result = ESaviorResult::Failed;
	return;} World = Context->GetWorld();
	//
	for (TActorIterator<AActor>Actor(World); Actor; ++Actor) {
		if ((*Actor)->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject|RF_BeginDestroyed)) {continue;}
		if (!Actor->IsA(Class.Get())) {continue;}
		//
		if (!LoadHierarchy) {LoadActor(World,*Actor,Result);}
		else {LoadActorHierarchy(World,*Actor,Result);}
	}///
}

void USavior3::SaveGameInstanceSingleTon(UGameInstance* Instance, ESaviorResult &Result) {
	if (Instance==nullptr) {Result=ESaviorResult::Failed; return;}
	if (Instance->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject|RF_BeginDestroyed)) {Result=ESaviorResult::Failed; return;}
	//
	FString Patch;
	const auto ObjectID = Instance->GetFName();
	ObjectID.ToString().Split(TEXT("_C"),&Patch,nullptr,ESearchCase::CaseSensitive,ESearchDir::FromEnd);
	//
	if (Compression==ERecordType::Complex) {
		const auto Record = GenerateRecord_Object(Instance);
		SlotData->Complex.Add(*Patch,Record);
	} else {
		const auto Record = GenerateMinimalRecord_Object(Instance);
		SlotData->Minimal.Add(*Patch,Record);
	}///
	//
	Result = ESaviorResult::Success;
}

void USavior3::LoadGameInstanceSingleTon(UGameInstance* Instance, ESaviorResult &Result) {
	if (SlotData==nullptr) {Result=ESaviorResult::Failed; return;}
	if (Instance==nullptr) {Result=ESaviorResult::Failed; return;}
	if (Instance->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject|RF_BeginDestroyed)) {Result=ESaviorResult::Failed; return;}
	//
	FString Patch;
	const auto ObjectID = Instance->GetFName();
	ObjectID.ToString().Split(TEXT("_C_"),&Patch,nullptr,ESearchCase::CaseSensitive,ESearchDir::FromEnd);
	//
	if (Compression==ERecordType::Complex) {
		if (!SlotData->Complex.Contains(*Patch)) {Result=ESaviorResult::Failed; return;}
		const auto Record = SlotData->Complex.FindRef(*Patch);
		//
		if (Record.Destroyed) {
			Instance->ConditionalBeginDestroy();
			Result = ESaviorResult::Success;
		} else {UnpackRecord_Object(Record,Instance,Result);}
	} else {
		if (!SlotData->Minimal.Contains(*Patch)) {Result=ESaviorResult::Failed; return;}
		const auto Record = SlotData->Minimal.FindRef(*Patch);
		//
		if (Record.Destroyed) {
			Instance->ConditionalBeginDestroy();
			Result = ESaviorResult::Success;
		} else {UnpackMinimalRecord_Object(Record,Instance,Result);}
	}///
}

void USavior3::StaticLoadObject(UObject* Context, UObject* Object, ESaviorResult &Result) {
	if (Object==nullptr) {Result=ESaviorResult::Failed; return;}
	//
	if (Context==nullptr||Context->GetWorld()==nullptr||Context->IsPendingKill()) {Result=ESaviorResult::Failed; return;}
	if (Object->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject|RF_BeginDestroyed)) {Result=ESaviorResult::Failed; return;}
	//
	if (StaticSaviorSlot.Savior==nullptr) {NewStaticSlot(Context);}
	//
	if (StaticSaviorSlot.Savior->IsValidLowLevelFast()) {
		StaticSaviorSlot.Savior->LoadObject(Object,Result);
	}///
}

void USavior3::StaticLoadComponent(UObject* Context, UActorComponent* Component, ESaviorResult &Result) {
	if (Component==nullptr) {Result=ESaviorResult::Failed; return;}
	//
	if (TAG_CNOLOAD(Component)) {Result=ESaviorResult::Failed; return;}
	if (Component->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject|RF_BeginDestroyed)) {Result=ESaviorResult::Failed; return;}
	//
	if (StaticSaviorSlot.Savior==nullptr) {NewStaticSlot(Context);}
	//
	if (StaticSaviorSlot.Savior->IsValidLowLevelFast()) {
		StaticSaviorSlot.Savior->LoadComponent(Context,Component,Result);
	}///
}

void USavior3::StaticLoadActor(UObject* Context, AActor* Actor, ESaviorResult &Result) {
	if (Actor==nullptr) {Result=ESaviorResult::Failed; return;}
	//
	if (TAG_ANOLOAD(Actor)) {Result=ESaviorResult::Failed; return;}
	if (Actor->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject|RF_BeginDestroyed)) {Result=ESaviorResult::Failed; return;}
	//
	if (StaticSaviorSlot.Savior==nullptr) {NewStaticSlot(Context);}
	//
	if (StaticSaviorSlot.Savior->IsValidLowLevelFast()) {
		StaticSaviorSlot.Savior->LoadActor(Context,Actor,Result);
	}///
}

void USavior3::LoadObjectData(UObject* Object, const FSaviorRecord &Data, ESaviorResult &Result) {
	if (Object==nullptr) {Result=ESaviorResult::Failed; return;}
	//
	if (Object->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject|RF_BeginDestroyed)) {Result=ESaviorResult::Failed; return;}
	if (StaticSaviorSlot.Savior==nullptr) {NewStaticSlot(Object);}
	//
	if (Data.Destroyed) {
		Object->ConditionalBeginDestroy();
	} else {
		TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
		TSharedRef<TJsonReader<TCHAR>>JReader = TJsonReaderFactory<TCHAR>::Create(Data.Buffer);
		//
		if (!FJsonSerializer::Deserialize(JReader,JSON)) {
			LOG_SV(true,ESeverity::Warning,FString::Printf(TEXT("[Data<->Buffer]: Corrupted Data. Unable to unpack Data Record for Object: [%s]"),*Object->GetName()));
		}///
		//
		for (TFieldIterator<FProperty>PIT(Object->GetClass(),EFieldIteratorFlags::IncludeSuper); PIT; ++PIT) {
			FProperty* Property = *PIT;
			//
			const auto PID = StaticSaviorSlot.Savior->GetMappedPropertyName(Object->GetClass(),Property->GetFName());
			if (!Property->HasAnyPropertyFlags(CPF_SaveGame)) {continue;}
			if (!IsSupportedProperty(Property)) {continue;}
			const FString ID = PID.ToString();
			//
			const bool IsSet = Property->IsA(FSetProperty::StaticClass());
			const bool IsMap = Property->IsA(FMapProperty::StaticClass());
			const bool IsInt = Property->IsA(FIntProperty::StaticClass());
			const bool IsBool = Property->IsA(FBoolProperty::StaticClass());
			const bool IsByte = Property->IsA(FByteProperty::StaticClass());
			const bool IsEnum = Property->IsA(FEnumProperty::StaticClass());
			const bool IsName = Property->IsA(FNameProperty::StaticClass());
			const bool IsText = Property->IsA(FTextProperty::StaticClass());
			const bool IsString = Property->IsA(FStrProperty::StaticClass());
			const bool IsArray = Property->IsA(FArrayProperty::StaticClass());
			const bool IsFloat = Property->IsA(FFloatProperty::StaticClass());
			const bool IsInt64 = Property->IsA(FInt64Property::StaticClass());
			const bool IsClass = Property->IsA(FClassProperty::StaticClass());
			const bool IsStruct = Property->IsA(FStructProperty::StaticClass());
			const bool IsObject = Property->IsA(FObjectProperty::StaticClass());
			//
			if ((IsInt)&&(JSON->HasField(ID))) {JSONToFProperty(JSON,ID,CastFieldChecked<FIntProperty>(Property),Object); continue;}
			if ((IsBool)&&(JSON->HasField(ID))) {JSONToFProperty(JSON,ID,CastFieldChecked<FBoolProperty>(Property),Object); continue;}
			if ((IsByte)&&(JSON->HasField(ID))) {JSONToFProperty(JSON,ID,CastFieldChecked<FByteProperty>(Property),Object); continue;}
			if ((IsEnum)&&(JSON->HasField(ID))) {JSONToFProperty(JSON,ID,CastFieldChecked<FEnumProperty>(Property),Object); continue;}
			if ((IsName)&&(JSON->HasField(ID))) {JSONToFProperty(JSON,ID,CastFieldChecked<FNameProperty>(Property),Object); continue;}
			if ((IsText)&&(JSON->HasField(ID))) {JSONToFProperty(JSON,ID,CastFieldChecked<FTextProperty>(Property),Object); continue;}
			if ((IsString)&&(JSON->HasField(ID))) {JSONToFProperty(JSON,ID,CastFieldChecked<FStrProperty>(Property),Object); continue;}
			if ((IsFloat)&&(JSON->HasField(ID))) {JSONToFProperty(JSON,ID,CastFieldChecked<FFloatProperty>(Property),Object); continue;}
			if ((IsInt64)&&(JSON->HasField(ID))) {JSONToFProperty(JSON,ID,CastFieldChecked<FInt64Property>(Property),Object); continue;}
			//
			if ((IsSet)&&(JSON->HasField(ID))) {JSONToFProperty(StaticSaviorSlot.Savior,JSON,ID,CastFieldChecked<FSetProperty>(Property),Object); continue;}
			if ((IsMap)&&(JSON->HasField(ID))) {JSONToFProperty(StaticSaviorSlot.Savior,JSON,ID,CastFieldChecked<FMapProperty>(Property),Object); continue;}
			if ((IsArray)&&(JSON->HasField(ID))) {JSONToFProperty(StaticSaviorSlot.Savior,JSON,ID,CastFieldChecked<FArrayProperty>(Property),Object); continue;}
			if ((IsClass)&&(JSON->HasField(ID))) {JSONToFProperty(StaticSaviorSlot.Savior,JSON,ID,CastFieldChecked<FClassProperty>(Property),Object); continue;}
			if ((IsObject)&&(JSON->HasField(ID))) {JSONToFProperty(StaticSaviorSlot.Savior,JSON,ID,CastFieldChecked<FObjectProperty>(Property),Object); continue;}
			//
			if ((IsStruct)&&(CastFieldChecked<FStructProperty>(Property)->Struct==TBaseStructure<FGuid>::Get())&&(Property->GetName().Equals(TEXT("SGUID"),ESearchCase::IgnoreCase))) {continue;} else
			if ((IsStruct)&&(JSON->HasField(ID))&&(CastFieldChecked<FStructProperty>(Property)->Struct==TBaseStructure<FTransform>::Get())) {JSONToFProperty(JSON,ID,CastFieldChecked<FStructProperty>(Property),Object,EStructType::Transform); continue;} else
			if ((IsStruct)&&(JSON->HasField(ID))&&(CastFieldChecked<FStructProperty>(Property)->Struct==TBaseStructure<FLinearColor>::Get())) {JSONToFProperty(JSON,ID,CastFieldChecked<FStructProperty>(Property),Object,EStructType::LColor); continue;} else
			if ((IsStruct)&&(JSON->HasField(ID))&&(CastFieldChecked<FStructProperty>(Property)->Struct==TBaseStructure<FVector2D>::Get())) {JSONToFProperty(JSON,ID,CastFieldChecked<FStructProperty>(Property),Object,EStructType::Vector2D); continue;} else
			if ((IsStruct)&&(JSON->HasField(ID))&&(CastFieldChecked<FStructProperty>(Property)->Struct==FRuntimeFloatCurve::StaticStruct())) {JSONToFProperty(JSON,ID,CastFieldChecked<FStructProperty>(Property),Object,EStructType::Curve); continue;} else
			if ((IsStruct)&&(JSON->HasField(ID))&&(CastFieldChecked<FStructProperty>(Property)->Struct==TBaseStructure<FRotator>::Get())) {JSONToFProperty(JSON,ID,CastFieldChecked<FStructProperty>(Property),Object,EStructType::Rotator); continue;} else
			if ((IsStruct)&&(JSON->HasField(ID))&&(CastFieldChecked<FStructProperty>(Property)->Struct==TBaseStructure<FVector>::Get())) {JSONToFProperty(JSON,ID,CastFieldChecked<FStructProperty>(Property),Object,EStructType::Vector3D); continue;} else
			if ((IsStruct)&&(JSON->HasField(ID))&&(CastFieldChecked<FStructProperty>(Property)->Struct==TBaseStructure<FColor>::Get())) {JSONToFProperty(JSON,ID,CastFieldChecked<FStructProperty>(Property),Object,EStructType::FColor); continue;} else
			if ((IsStruct)&&(JSON->HasField(ID))) {JSONToFProperty(JSON,ID,CastFieldChecked<FStructProperty>(Property),Object,EStructType::Struct); continue;}
		}///
		//
		//
		if (Object->IsA(UActorComponent::StaticClass())) {
			auto Component = CastChecked<UActorComponent>(Object);
			if (IsInGameThread()) {DeserializeComponent(Data,Component);} else {
				FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
					FSimpleDelegateGraphTask::FDelegate::CreateStatic(&DeserializeComponent,Data,Component),
					GET_STATID(STAT_FSimpleDelegateGraphTask_DeserializeEntity),
					nullptr, ENamedThreads::GameThread
				);//
			}///
			//
			FString ID = Reflector::MakeComponentID(Component).ToString();
			LOG_SV(StaticSaviorSlot.Savior->DeepLogs,ESeverity::Info,FString::Printf(TEXT("UNPACKED DATA for %s :: "),*ID)+Data.Buffer);
			LOG_SV(StaticSaviorSlot.Savior->Debug,ESeverity::Info,FString("Deserialized :: ")+ID);
		} else if (Object->IsA(AActor::StaticClass())) {
			auto Actor = CastChecked<AActor>(Object);
			if (IsInGameThread()) {DeserializeActor(Data,Actor);} else {
				FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
					FSimpleDelegateGraphTask::FDelegate::CreateStatic(&DeserializeActor,Data,Actor),
					GET_STATID(STAT_FSimpleDelegateGraphTask_DeserializeEntity),
					nullptr, ENamedThreads::GameThread
				);//
			}///
			//
			FString ID = Reflector::MakeActorID(Actor).ToString();
			LOG_SV(StaticSaviorSlot.Savior->DeepLogs,ESeverity::Info,FString::Printf(TEXT("UNPACKED DATA for %s :: "),*ID)+Data.Buffer);
			LOG_SV(StaticSaviorSlot.Savior->Debug,ESeverity::Info,FString("Deserialized :: ")+ID);
		} else {
			FString ID = Reflector::MakeObjectID(Object).ToString();
			LOG_SV(StaticSaviorSlot.Savior->DeepLogs,ESeverity::Info,FString::Printf(TEXT("UNPACKED DATA for %s :: "),*ID)+Data.Buffer);
			LOG_SV(StaticSaviorSlot.Savior->Debug,ESeverity::Info,FString("Deserialized :: ")+ID);
		}///
	}///
	//
	Result = ESaviorResult::Success;
}

void USavior3::LoadComponentData(UActorComponent* Component, const FSaviorRecord &Data, ESaviorResult &Result) {
	if (Component==nullptr) {Result=ESaviorResult::Failed; return;}
	//
	if (TAG_CNOLOAD(Component)) {Result=ESaviorResult::Failed; return;}
	if (Component->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject|RF_BeginDestroyed)) {Result=ESaviorResult::Failed; return;}
	//
	if (StaticSaviorSlot.Savior==nullptr) {NewStaticSlot(Component);}
	//
	if (!TAG_CNOKILL(Component)&&(Data.Destroyed)) {
		Component->DestroyComponent();
		Result = ESaviorResult::Success;
	} else {LoadObjectData(Component,Data,Result);}
}

void USavior3::LoadActorData(AActor* Actor, const FSaviorRecord &Data, ESaviorResult &Result) {
	if (Actor==nullptr) {Result=ESaviorResult::Failed; return;}
	//
	if (TAG_ANOLOAD(Actor)) {Result=ESaviorResult::Failed; return;}
	if (Actor->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject|RF_BeginDestroyed)) {Result=ESaviorResult::Failed; return;}
	//
	if (StaticSaviorSlot.Savior==nullptr) {NewStaticSlot(Actor);}
	//
	if (!TAG_ANOKILL(Actor)&&(Data.Destroyed)) {
		Actor->Destroy();
		Result = ESaviorResult::Success;
	} else {LoadObjectData(Actor,Data,Result);}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Savior World Serialization Interface:

void USavior3::SaveGameWorld(UObject* Context, ESaviorResult &Result) {
	if (Context->GetWorld()==nullptr) {Result=ESaviorResult::Failed; return;}
	CheckInstance();
	//
	if (USavior3::ThreadSafety!=EThreadSafety::IsCurrentlyThreadSafe) {
		EVENT_OnFinishDataLOAD.Broadcast(false);
		Result=ESaviorResult::Failed;
	return;}
	//
	Result = PrepareSlotToSave(Context);
	if (Result==ESaviorResult::Failed) {return;}
	//
	switch (RuntimeMode) {
		case ERuntimeMode::SynchronousTask:
		{
			USavior3::LastThreadState = USavior3::ThreadSafety;
			USavior3::ThreadSafety = EThreadSafety::SynchronousSaving;
			(new FAutoDeleteAsyncTask<TASK_SerializeGameWorld>(this))->StartSynchronousTask();
		}	break;
		case ERuntimeMode::BackgroundTask:
		{
			USavior3::LastThreadState = ThreadSafety;
			USavior3::ThreadSafety = EThreadSafety::AsynchronousSaving;
			(new FAutoDeleteAsyncTask<TASK_SerializeGameWorld>(this))->StartBackgroundTask();
		}	break;
	default: break;}
}

void USavior3::LoadGameWorld(UObject* Context, const bool ResetLevelOnLoad, ESaviorResult &Result) {
	if (Context->GetWorld()==nullptr) {Result=ESaviorResult::Failed; return;}
	if (SlotData==nullptr) {Result=ESaviorResult::Failed; return;}
	CheckInstance();
	//
	if (USavior3::ThreadSafety!=EThreadSafety::IsCurrentlyThreadSafe) {
		EVENT_OnFinishDataLOAD.Broadcast(false);
		Result=ESaviorResult::Failed;
	return;}
	//
	const auto &Settings = GetDefault<USavior3Settings>();
	ReadSlotFromFile(Settings->DefaultPlayerID,Result);
	if (Result==ESaviorResult::Failed) {return;}
	//
	Result = PrepareSlotToLoad(Context);
	if (Result==ESaviorResult::Failed) {return;}
	//
	FreeStaticSlot(GetFName());
	if (Context->GetWorld()->GetFName()!=(*SlotMeta->SaveLocation)) {
		StaticSaviorSlot.Location = *SlotMeta->SaveLocation;
		NewStaticSlot(Context,this);
		//
		UGameplayStatics::OpenLevel(Context,*SlotMeta->SaveLocation,true); return;
	} else if (ResetLevelOnLoad && Context->GetWorld()) {
		const auto &GameMode = Context->GetWorld()->GetAuthGameMode();
		if ((GameMode!=nullptr)&&(GameMode->IsValidLowLevelFast())) {
			GameMode->ResetLevel();
		}///
	}///
	//
	//
	switch (RuntimeMode) {
		case ERuntimeMode::SynchronousTask:
		{
			USavior3::LastThreadState = USavior3::ThreadSafety;
			USavior3::ThreadSafety = EThreadSafety::SynchronousLoading;
			(new FAutoDeleteAsyncTask<TASK_DeserializeGameWorld>(this))->StartSynchronousTask();
		}	break;
		case ERuntimeMode::BackgroundTask:
		{
			USavior3::LastThreadState = USavior3::ThreadSafety;
			USavior3::ThreadSafety = EThreadSafety::AsynchronousLoading;
			(new FAutoDeleteAsyncTask<TASK_DeserializeGameWorld>(this))->StartBackgroundTask();
		}	break;
	default: break;}
}

void USavior3::SaveLevel(UObject* Context, FName LevelToSave, ESaviorResult &Result) {
	if (Context==nullptr||Context->GetWorld()==nullptr) {Result=ESaviorResult::Failed; return;}
	CheckInstance();
	//
	if (USavior3::ThreadSafety!=EThreadSafety::IsCurrentlyThreadSafe) {
		EVENT_OnFinishDataLOAD.Broadcast(false);
		Result=ESaviorResult::Failed;
	return;}
	//
	Result = PrepareSlotToSave(Context);
	if (Result==ESaviorResult::Failed) {return;}
	//
	switch (RuntimeMode) {
		case ERuntimeMode::SynchronousTask:
		{
			USavior3::LastThreadState = USavior3::ThreadSafety;
			USavior3::ThreadSafety = EThreadSafety::SynchronousSaving;
			(new FAutoDeleteAsyncTask<TASK_SerializeLevel>(this,LevelToSave))->StartSynchronousTask();
		}	break;
		case ERuntimeMode::BackgroundTask:
		{
			USavior3::LastThreadState = USavior3::ThreadSafety;
			USavior3::ThreadSafety = EThreadSafety::AsynchronousSaving;
			(new FAutoDeleteAsyncTask<TASK_SerializeLevel>(this,LevelToSave))->StartBackgroundTask();
		}	break;
	default: break;}
}

void USavior3::LoadLevel(UObject* Context, FName LevelToLoad, ESaviorResult &Result) {
	if (Context==nullptr||Context->GetWorld()==nullptr) {Result=ESaviorResult::Failed; return;}
	CheckInstance();
	//
	if (USavior3::ThreadSafety!=EThreadSafety::IsCurrentlyThreadSafe) {
		EVENT_OnFinishDataLOAD.Broadcast(false);
		Result=ESaviorResult::Failed;
	return;}
	//
	const auto &Settings = GetDefault<USavior3Settings>();
	ReadSlotFromFile(Settings->DefaultPlayerID,Result);
	if (Result==ESaviorResult::Failed) {return;}
	//
	Result = PrepareSlotToLoad(Context);
	if (Result==ESaviorResult::Failed) {return;}
	//
	//
	switch (RuntimeMode) {
		case ERuntimeMode::SynchronousTask:
		{
			USavior3::LastThreadState = USavior3::ThreadSafety;
			USavior3::ThreadSafety = EThreadSafety::SynchronousLoading;
			(new FAutoDeleteAsyncTask<TASK_DeserializeLevel>(this,LevelToLoad))->StartSynchronousTask();
		}	break;
		case ERuntimeMode::BackgroundTask:
		{
			USavior3::LastThreadState = USavior3::ThreadSafety;
			USavior3::ThreadSafety = EThreadSafety::AsynchronousLoading;
			(new FAutoDeleteAsyncTask<TASK_DeserializeLevel>(this,LevelToLoad))->StartBackgroundTask();
		}	break;
	default: break;}
}

void USavior3::SaveGameMode(UObject* Context, ESaviorResult &Result) {
	if (Context==nullptr||Context->GetWorld()==nullptr) {Result=ESaviorResult::Failed; return;}
	CheckInstance();
	//
	if (USavior3::ThreadSafety!=EThreadSafety::IsCurrentlyThreadSafe) {
		EVENT_OnFinishDataLOAD.Broadcast(false);
		Result=ESaviorResult::Failed;
	return;}
	//
	Result = PrepareSlotToSave(Context);
	if (Result==ESaviorResult::Failed) {return;}
	//
	switch (RuntimeMode) {
		case ERuntimeMode::SynchronousTask:
		{
			USavior3::LastThreadState = USavior3::ThreadSafety;
			USavior3::ThreadSafety = EThreadSafety::SynchronousSaving;
			(new FAutoDeleteAsyncTask<TASK_SerializeGameMode>(this))->StartSynchronousTask();
		}	break;
		case ERuntimeMode::BackgroundTask:
		{
			USavior3::LastThreadState = USavior3::ThreadSafety;
			USavior3::ThreadSafety = EThreadSafety::AsynchronousSaving;
			(new FAutoDeleteAsyncTask<TASK_SerializeGameMode>(this))->StartBackgroundTask();
		}	break;
	default: break;}
}

void USavior3::LoadGameMode(UObject* Context, ESaviorResult &Result) {
	if (Context==nullptr||Context->GetWorld()==nullptr) {Result=ESaviorResult::Failed; return;}
	CheckInstance();
	//
	if (USavior3::ThreadSafety!=EThreadSafety::IsCurrentlyThreadSafe) {
		EVENT_OnFinishDataLOAD.Broadcast(false);
		Result=ESaviorResult::Failed;
	return;}
	//
	const auto &Settings = GetDefault<USavior3Settings>();
	ReadSlotFromFile(Settings->DefaultPlayerID,Result);
	if (Result==ESaviorResult::Failed) {return;}
	//
	Result = PrepareSlotToLoad(Context);
	if (Result==ESaviorResult::Failed) {return;}
	//
	//
	switch (RuntimeMode) {
		case ERuntimeMode::SynchronousTask:
		{
			USavior3::LastThreadState = USavior3::ThreadSafety;
			USavior3::ThreadSafety = EThreadSafety::SynchronousLoading;
			(new FAutoDeleteAsyncTask<TASK_DeserializeGameMode>(this))->StartSynchronousTask();
		}	break;
		case ERuntimeMode::BackgroundTask:
		{
			USavior3::LastThreadState = USavior3::ThreadSafety;
			USavior3::ThreadSafety = EThreadSafety::AsynchronousLoading;
			(new FAutoDeleteAsyncTask<TASK_DeserializeGameMode>(this))->StartBackgroundTask();
		}	break;
	default: break;}
}

void USavior3::SaveGameInstance(UObject* Context, ESaviorResult &Result) {
	if (Context==nullptr||Context->GetWorld()==nullptr) {Result=ESaviorResult::Failed; return;}
	CheckInstance();
	//
	if (USavior3::ThreadSafety!=EThreadSafety::IsCurrentlyThreadSafe) {
		EVENT_OnFinishDataLOAD.Broadcast(false);
		Result=ESaviorResult::Failed;
	return;}
	//
	Result = PrepareSlotToSave(Context);
	if (Result==ESaviorResult::Failed) {return;}
	//
	switch (RuntimeMode) {
		case ERuntimeMode::SynchronousTask:
		{
			USavior3::LastThreadState = USavior3::ThreadSafety;
			USavior3::ThreadSafety = EThreadSafety::SynchronousSaving;
			(new FAutoDeleteAsyncTask<TASK_SerializeGameInstance>(this))->StartSynchronousTask();
		}	break;
		case ERuntimeMode::BackgroundTask:
		{
			USavior3::LastThreadState = USavior3::ThreadSafety;
			USavior3::ThreadSafety = EThreadSafety::AsynchronousSaving;
			(new FAutoDeleteAsyncTask<TASK_SerializeGameInstance>(this))->StartBackgroundTask();
		}	break;
	default: break;}
}

void USavior3::LoadGameInstance(UObject* Context, ESaviorResult &Result) {
	if (Context==nullptr||Context->GetWorld()==nullptr) {Result=ESaviorResult::Failed; return;}
	CheckInstance();
	//
	if (USavior3::ThreadSafety!=EThreadSafety::IsCurrentlyThreadSafe) {
		EVENT_OnFinishDataLOAD.Broadcast(false);
		Result=ESaviorResult::Failed;
	return;}
	//
	const auto &Settings = GetDefault<USavior3Settings>();
	ReadSlotFromFile(Settings->DefaultPlayerID,Result);
	if (Result==ESaviorResult::Failed) {return;}
	//
	Result = PrepareSlotToLoad(Context);
	if (Result==ESaviorResult::Failed) {return;}
	//
	//
	switch (RuntimeMode) {
		case ERuntimeMode::SynchronousTask:
		{
			USavior3::LastThreadState = USavior3::ThreadSafety;
			USavior3::ThreadSafety = EThreadSafety::SynchronousLoading;
			(new FAutoDeleteAsyncTask<TASK_DeserializeGameInstance>(this))->StartSynchronousTask();
		}	break;
		case ERuntimeMode::BackgroundTask:
		{
			USavior3::LastThreadState = USavior3::ThreadSafety;
			USavior3::ThreadSafety = EThreadSafety::AsynchronousLoading;
			(new FAutoDeleteAsyncTask<TASK_DeserializeGameInstance>(this))->StartBackgroundTask();
		}	break;
	default: break;}
}

void USavior3::StaticLoadGameWorld(UWorld* InWorld) {
	if (StaticSaviorSlot.Slot != GetFName()) {return;}
	if (StaticSaviorSlot.Location != InWorld->GetFName()) {return;}
	//
	ESaviorResult Result;
	StaticSaviorSlot.Savior = NewSlotInstance(InWorld,this,Result);
	//
	if (StaticSaviorSlot.Savior->IsValidLowLevelFast()) {
		USavior3::LastThreadState = EThreadSafety::IsCurrentlyThreadSafe;
		USavior3::ThreadSafety = EThreadSafety::IsCurrentlyThreadSafe;
		//
		StaticSaviorSlot.Savior->LoadGameWorld(InWorld,false,Result);
	}///
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Savior Object Serialization Interface:

///	(C) 2020 - Bruno Xavier B. Leite
FSaviorRecord USavior3::GenerateRecord_Object(const UObject* Object) {
	FSaviorRecord Record;
	FString Buffer;
	//
	TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
	TSharedRef<TJsonWriter<TCHAR,TCondensedJsonPrintPolicy<TCHAR>>>JBuffer = TJsonWriterFactory<TCHAR,TCondensedJsonPrintPolicy<TCHAR>>::Create(&Buffer);
	//
	if (Object==nullptr) {return Record;}
	//
	//
	for (TFieldIterator<FProperty>PIT(Object->GetClass(),EFieldIteratorFlags::IncludeSuper); PIT; ++PIT) {
		FProperty* Property = *PIT;
		//
		if (!IsSupportedProperty(Property)) {continue;}
		if (Property->HasAnyPropertyFlags(CPF_Deprecated)) {continue;}
		//
		if ((Object->IsA(ULightComponent::StaticClass()))) {
			if (!Property->HasAnyPropertyFlags(CPF_BlueprintVisible)) {continue;}
		} else {
			if (!Property->HasAnyPropertyFlags(CPF_SaveGame)) {continue;}
		}///
		//
		const auto PID = GetMappedPropertyName(Object->GetClass(),Property->GetFName());
		const FString ID = PID.ToString();
		//
		const bool IsSet = Property->IsA(FSetProperty::StaticClass());
		const bool IsMap = Property->IsA(FMapProperty::StaticClass());
		const bool IsInt = Property->IsA(FIntProperty::StaticClass());
		const bool IsBool = Property->IsA(FBoolProperty::StaticClass());
		const bool IsByte = Property->IsA(FByteProperty::StaticClass());
		const bool IsEnum = Property->IsA(FEnumProperty::StaticClass());
		const bool IsName = Property->IsA(FNameProperty::StaticClass());
		const bool IsText = Property->IsA(FTextProperty::StaticClass());
		const bool IsString = Property->IsA(FStrProperty::StaticClass());
		const bool IsInt64 = Property->IsA(FInt64Property::StaticClass());
		const bool IsArray = Property->IsA(FArrayProperty::StaticClass());
		const bool IsFloat = Property->IsA(FFloatProperty::StaticClass());
		const bool IsClass = Property->IsA(FClassProperty::StaticClass());
		const bool IsStruct = Property->IsA(FStructProperty::StaticClass());
		const bool IsObject = Property->IsA(FObjectProperty::StaticClass());
		//
		if (IsBool && ID.Equals(TEXT("Destroyed"),ESearchCase::IgnoreCase)) {
			Record.Destroyed = CastFieldChecked<FBoolProperty>(Property)->GetPropertyValue_InContainer(Object);
		continue;}
		//
		if (IsInt) {JSON->SetField(ID,UPropertyToJSON(CastFieldChecked<FIntProperty>(Property),Object)); continue;}
		if (IsBool) {JSON->SetField(ID,UPropertyToJSON(CastFieldChecked<FBoolProperty>(Property),Object)); continue;}
		if (IsByte) {JSON->SetField(ID,UPropertyToJSON(CastFieldChecked<FByteProperty>(Property),Object)); continue;}
		if (IsEnum) {JSON->SetField(ID,UPropertyToJSON(CastFieldChecked<FEnumProperty>(Property),Object)); continue;}
		if (IsName) {JSON->SetField(ID,UPropertyToJSON(CastFieldChecked<FNameProperty>(Property),Object)); continue;}
		if (IsText) {JSON->SetField(ID,UPropertyToJSON(CastFieldChecked<FTextProperty>(Property),Object)); continue;}
		if (IsString) {JSON->SetField(ID,UPropertyToJSON(CastFieldChecked<FStrProperty>(Property),Object)); continue;}
		if (IsFloat) {JSON->SetField(ID,UPropertyToJSON(CastFieldChecked<FFloatProperty>(Property),Object)); continue;}
		if (IsInt64) {JSON->SetField(ID,UPropertyToJSON(CastFieldChecked<FInt64Property>(Property),Object)); continue;}
		if (IsClass) {JSON->SetObjectField(ID,UPropertyToJSON(CastFieldChecked<FClassProperty>(Property),Object)); continue;}
		if (IsObject) {JSON->SetObjectField(ID,UPropertyToJSON(CastFieldChecked<FObjectProperty>(Property),Object)); continue;}
		//
		if (IsSet) {JSON->SetArrayField(ID,UPropertyToJSON(CastFieldChecked<FSetProperty>(Property),Object)); continue;}
		if (IsMap) {JSON->SetObjectField(ID,UPropertyToJSON(CastFieldChecked<FMapProperty>(Property),Object)); continue;}
		if (IsArray) {JSON->SetArrayField(ID,UPropertyToJSON(CastFieldChecked<FArrayProperty>(Property),Object)); continue;}
		//
		if ((IsStruct)&&(CastFieldChecked<FStructProperty>(Property)->Struct==TBaseStructure<FGuid>::Get())&&(Property->GetName().Equals(TEXT("SGUID"),ESearchCase::IgnoreCase))) {continue;} else
		if ((IsStruct)&&(CastFieldChecked<FStructProperty>(Property)->Struct==TBaseStructure<FTransform>::Get())) {JSON->SetObjectField(ID,UPropertyToJSON(CastFieldChecked<FStructProperty>(Property),Object,EStructType::Transform)); continue;} else
		if ((IsStruct)&&(CastFieldChecked<FStructProperty>(Property)->Struct==TBaseStructure<FLinearColor>::Get())) {JSON->SetObjectField(ID,UPropertyToJSON(CastFieldChecked<FStructProperty>(Property),Object,EStructType::LColor)); continue;} else
		if ((IsStruct)&&(CastFieldChecked<FStructProperty>(Property)->Struct==TBaseStructure<FVector2D>::Get())) {JSON->SetObjectField(ID,UPropertyToJSON(CastFieldChecked<FStructProperty>(Property),Object,EStructType::Vector2D)); continue;} else
		if ((IsStruct)&&(CastFieldChecked<FStructProperty>(Property)->Struct==FRuntimeFloatCurve::StaticStruct())) {JSON->SetObjectField(ID,UPropertyToJSON(CastFieldChecked<FStructProperty>(Property),Object,EStructType::Curve)); continue;} else
		if ((IsStruct)&&(CastFieldChecked<FStructProperty>(Property)->Struct==TBaseStructure<FRotator>::Get())) {JSON->SetObjectField(ID,UPropertyToJSON(CastFieldChecked<FStructProperty>(Property),Object,EStructType::Rotator)); continue;} else
		if ((IsStruct)&&(CastFieldChecked<FStructProperty>(Property)->Struct==TBaseStructure<FVector>::Get())) {JSON->SetObjectField(ID,UPropertyToJSON(CastFieldChecked<FStructProperty>(Property),Object,EStructType::Vector3D)); continue;} else
		if ((IsStruct)&&(CastFieldChecked<FStructProperty>(Property)->Struct==TBaseStructure<FColor>::Get())) {JSON->SetObjectField(ID,UPropertyToJSON(CastFieldChecked<FStructProperty>(Property),Object,EStructType::FColor)); continue;} else
		if ((IsStruct)) {JSON->SetObjectField(ID,UPropertyToJSON(CastFieldChecked<FStructProperty>(Property),Object,EStructType::Struct)); continue;}
	} FJsonSerializer::Serialize(JSON.ToSharedRef(),JBuffer);
	//
	//
	Record.Active = true;
	Record.GUID = FindGUID(Object);
	Record.ClassPath = Object->GetClass()->GetDefaultObject()->GetPathName();
	//
	const auto &Settings = GetDefault<USavior3Settings>();
	//
	if (Object->IsA(UActorComponent::StaticClass())) {
		auto Component = CastChecked<UActorComponent>(Object);
		const auto &Scene = Cast<USceneComponent>(Component);
		//
		Record.FullName = Reflector::MakeComponentID(const_cast<UActorComponent*>(Component)).ToString();
		Record.Active = Component->IsActive();
		//
		if (!TAG_CNOTRAN(Component) && (Scene && Scene->Mobility==EComponentMobility::Movable)) {
			Record.Scale = Scene->GetRelativeTransform().GetScale3D();
			Record.Location = Scene->GetRelativeTransform().GetLocation();
			Record.Rotation = Scene->GetRelativeTransform().GetRotation().Rotator();
		}///
		//
		if (!TAG_CNODATA(Component)) {Record.Buffer=Buffer;}
		if (TAG_CNOKILL(Component)) {Record.Destroyed=false;}
		//
		if (Component->GetOuter()!=nullptr) {
			Record.OuterName = Reflector::MakeObjectID(Component->GetOuter()).ToString();
		}///
		//
		LOG_SV(Debug,ESeverity::Info,FString("Serialized Component :: ")+Record.FullName);
	} else if (Object->IsA(AActor::StaticClass())) {
		auto Actor = CastChecked<AActor>(Object);
		//
		Record.FullName = Reflector::MakeActorID(const_cast<AActor*>(Actor),true).ToString();
		//
		if (const auto &Avatar = Cast<ACharacter>(Actor)) {
			if (const auto &Skel = Avatar->GetMesh()) {
				Record.ActorMesh = TSoftObjectPtr<USkeletalMesh>(Skel->SkeletalMesh).ToString();
			}///
		} else if (const auto &Mesh=Cast<UStaticMeshComponent>(Actor->GetRootComponent())) {
			Record.ActorMesh = TSoftObjectPtr<UStaticMesh>(Mesh->GetStaticMesh()).ToString();
		}///
		//
		if (!TAG_ANOTRAN(Actor)) {
			if (auto*Parent=Actor->GetParentActor()) {
				Record.Scale = Actor->GetActorTransform().GetRelativeTransform(Parent->GetActorTransform()).GetScale3D();
				Record.Location = Actor->GetActorTransform().GetRelativeTransform(Parent->GetActorTransform()).GetLocation();
				Record.Rotation = Actor->GetActorTransform().GetRelativeTransform(Parent->GetActorTransform()).GetRotation().Rotator();
			} else {
				Record.Scale = Actor->GetActorTransform().GetScale3D();
				Record.Location = Actor->GetActorTransform().GetLocation();
				Record.Rotation = Actor->GetActorTransform().GetRotation().Rotator();
			}///
		}///
		//
		if (!TAG_ANOPHYS(Actor) && Actor->GetRootComponent()) {
			Record.LinearVelocity = Actor->GetVelocity();
			//
			if (const auto &Primitive = Cast<UPrimitiveComponent>(Actor->GetRootComponent())) {
				Record.AngularVelocity=Primitive->GetPhysicsAngularVelocityInDegrees();
				Record.Active = Primitive->IsActive();
			}///
		}///
		//
		if (!TAG_ANODATA(Actor)) {Record.Buffer=Buffer;}
		if (TAG_ANOKILL(Actor)) {Record.Destroyed=false;}
		if (!TAG_ANOHIDE(Actor)) {Record.HiddenInGame=Actor->IsHidden();}
		//
		if (auto*Parent=Actor->GetParentActor()) {
			Record.OuterName = Reflector::MakeActorID(Parent).ToString();
		}///
		//
		LOG_SV(Debug,ESeverity::Info,FString("Serialized Actor :: ")+Record.FullName);
	} else {
		Record.Buffer = Buffer;
		if (Object->GetOuter()!=nullptr) {
			Record.OuterName = Reflector::MakeObjectID(Object->GetOuter()).ToString();
		}///
	}///
	//
	//
	LOG_SV(DeepLogs,ESeverity::Info,FString::Printf(TEXT("SAVED DATA for %s :: "),*Reflector::MakeObjectID(const_cast<UObject*>(Object)).ToString())+Buffer);
	//
	return Record;
}

FSaviorRecord USavior3::GenerateRecord_Component(const UActorComponent* Component) {
	return GenerateRecord_Object(Component);
}

FSaviorRecord USavior3::GenerateRecord_Actor(const AActor* Actor) {
	return GenerateRecord_Object(Actor);
}

///	(C) 2020 - Bruno Xavier B. Leite
void USavior3::UnpackRecord_Object(const FSaviorRecord &Record, UObject* Object, ESaviorResult &Result) {
	if (Object==nullptr||Object->IsPendingKill()) {Result=ESaviorResult::Failed; return;}
	const auto &Settings = GetDefault<USavior3Settings>();
	//
	TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
	TSharedRef<TJsonReader<TCHAR>>JReader = TJsonReaderFactory<TCHAR>::Create(Record.Buffer);
	//
	if (!FJsonSerializer::Deserialize(JReader,JSON)) {
		LOG_SV(Debug,ESeverity::Warning,FString::Printf(TEXT("[Data<->Buffer]: Corrupted Data. Unable to unpack Data Record for Object: [%s]"),*Object->GetName()));
	}///
	//
	//
	for (TFieldIterator<FProperty>PIT(Object->GetClass(),EFieldIteratorFlags::IncludeSuper); PIT; ++PIT) {
		FProperty* Property = *PIT;
		//
		if (!IsSupportedProperty(Property)) {continue;}
		if (Property->HasAnyPropertyFlags(CPF_Deprecated)) {continue;}
		//
		if ((Object->IsA(ULightComponent::StaticClass()))) {
			if (!Property->HasAnyPropertyFlags(CPF_BlueprintVisible)) {continue;}
		} else {
			if (!Property->HasAnyPropertyFlags(CPF_SaveGame)) {continue;}
		}///
		//
		const auto PID = GetMappedPropertyName(Object->GetClass(),Property->GetFName());
		const FString ID = PID.ToString();
		//
		const bool IsSet = Property->IsA(FSetProperty::StaticClass());
		const bool IsMap = Property->IsA(FMapProperty::StaticClass());
		const bool IsInt = Property->IsA(FIntProperty::StaticClass());
		const bool IsBool = Property->IsA(FBoolProperty::StaticClass());
		const bool IsByte = Property->IsA(FByteProperty::StaticClass());
		const bool IsEnum = Property->IsA(FEnumProperty::StaticClass());
		const bool IsName = Property->IsA(FNameProperty::StaticClass());
		const bool IsText = Property->IsA(FTextProperty::StaticClass());
		const bool IsString = Property->IsA(FStrProperty::StaticClass());
		const bool IsArray = Property->IsA(FArrayProperty::StaticClass());
		const bool IsFloat = Property->IsA(FFloatProperty::StaticClass());
		const bool IsInt64 = Property->IsA(FInt64Property::StaticClass());
		const bool IsClass = Property->IsA(FClassProperty::StaticClass());
		const bool IsStruct = Property->IsA(FStructProperty::StaticClass());
		const bool IsObject = Property->IsA(FObjectProperty::StaticClass());
		//
		if ((IsInt)&&(JSON->HasField(ID))) {JSONToFProperty(JSON,ID,CastFieldChecked<FIntProperty>(Property),Object); continue;}
		if ((IsBool)&&(JSON->HasField(ID))) {JSONToFProperty(JSON,ID,CastFieldChecked<FBoolProperty>(Property),Object); continue;}
		if ((IsByte)&&(JSON->HasField(ID))) {JSONToFProperty(JSON,ID,CastFieldChecked<FByteProperty>(Property),Object); continue;}
		if ((IsEnum)&&(JSON->HasField(ID))) {JSONToFProperty(JSON,ID,CastFieldChecked<FEnumProperty>(Property),Object); continue;}
		if ((IsName)&&(JSON->HasField(ID))) {JSONToFProperty(JSON,ID,CastFieldChecked<FNameProperty>(Property),Object); continue;}
		if ((IsText)&&(JSON->HasField(ID))) {JSONToFProperty(JSON,ID,CastFieldChecked<FTextProperty>(Property),Object); continue;}
		if ((IsString)&&(JSON->HasField(ID))) {JSONToFProperty(JSON,ID,CastFieldChecked<FStrProperty>(Property),Object); continue;}
		if ((IsFloat)&&(JSON->HasField(ID))) {JSONToFProperty(JSON,ID,CastFieldChecked<FFloatProperty>(Property),Object); continue;}
		if ((IsInt64)&&(JSON->HasField(ID))) {JSONToFProperty(JSON,ID,CastFieldChecked<FInt64Property>(Property),Object); continue;}
		//
		if ((IsSet)&&(JSON->HasField(ID))) {JSONToFProperty(this,JSON,ID,CastFieldChecked<FSetProperty>(Property),Object); continue;}
		if ((IsMap)&&(JSON->HasField(ID))) {JSONToFProperty(this,JSON,ID,CastFieldChecked<FMapProperty>(Property),Object); continue;}
		if ((IsArray)&&(JSON->HasField(ID))) {JSONToFProperty(this,JSON,ID,CastFieldChecked<FArrayProperty>(Property),Object); continue;}
		if ((IsClass)&&(JSON->HasField(ID))) {JSONToFProperty(this,JSON,ID,CastFieldChecked<FClassProperty>(Property),Object); continue;}
		if ((IsObject)&&(JSON->HasField(ID))) {JSONToFProperty(this,JSON,ID,CastFieldChecked<FObjectProperty>(Property),Object); continue;}
		//
		if ((IsStruct)&&(CastFieldChecked<FStructProperty>(Property)->Struct==TBaseStructure<FGuid>::Get())&&(Property->GetName().Equals(TEXT("SGUID"),ESearchCase::IgnoreCase))) {continue;} else
		if ((IsStruct)&&(JSON->HasField(ID))&&(CastFieldChecked<FStructProperty>(Property)->Struct==TBaseStructure<FTransform>::Get())) {JSONToFProperty(JSON,ID,CastFieldChecked<FStructProperty>(Property),Object,EStructType::Transform); continue;} else
		if ((IsStruct)&&(JSON->HasField(ID))&&(CastFieldChecked<FStructProperty>(Property)->Struct==TBaseStructure<FLinearColor>::Get())) {JSONToFProperty(JSON,ID,CastFieldChecked<FStructProperty>(Property),Object,EStructType::LColor); continue;} else
		if ((IsStruct)&&(JSON->HasField(ID))&&(CastFieldChecked<FStructProperty>(Property)->Struct==TBaseStructure<FVector2D>::Get())) {JSONToFProperty(JSON,ID,CastFieldChecked<FStructProperty>(Property),Object,EStructType::Vector2D); continue;} else
		if ((IsStruct)&&(JSON->HasField(ID))&&(CastFieldChecked<FStructProperty>(Property)->Struct==FRuntimeFloatCurve::StaticStruct())) {JSONToFProperty(JSON,ID,CastFieldChecked<FStructProperty>(Property),Object,EStructType::Curve); continue;} else
		if ((IsStruct)&&(JSON->HasField(ID))&&(CastFieldChecked<FStructProperty>(Property)->Struct==TBaseStructure<FRotator>::Get())) {JSONToFProperty(JSON,ID,CastFieldChecked<FStructProperty>(Property),Object,EStructType::Rotator); continue;} else
		if ((IsStruct)&&(JSON->HasField(ID))&&(CastFieldChecked<FStructProperty>(Property)->Struct==TBaseStructure<FVector>::Get())) {JSONToFProperty(JSON,ID,CastFieldChecked<FStructProperty>(Property),Object,EStructType::Vector3D); continue;} else
		if ((IsStruct)&&(JSON->HasField(ID))&&(CastFieldChecked<FStructProperty>(Property)->Struct==TBaseStructure<FColor>::Get())) {JSONToFProperty(JSON,ID,CastFieldChecked<FStructProperty>(Property),Object,EStructType::FColor); continue;} else
		if ((IsStruct)&&(JSON->HasField(ID))) {JSONToFProperty(JSON,ID,CastFieldChecked<FStructProperty>(Property),Object,EStructType::Struct); continue;}
	}///
	//
	//
	if (Object->IsA(UActorComponent::StaticClass())) {
		auto Component = CastChecked<UActorComponent>(Object);
		if (IsInGameThread()) {DeserializeComponent(Record,Component);} else {
			FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
				FSimpleDelegateGraphTask::FDelegate::CreateStatic(&DeserializeComponent,Record,Component),
				GET_STATID(STAT_FSimpleDelegateGraphTask_DeserializeEntity),
				nullptr, ENamedThreads::GameThread
			);//
		}///
		//
		FString ID = Reflector::MakeComponentID(Component).ToString();
		LOG_SV(DeepLogs,ESeverity::Info,FString::Printf(TEXT("UNPACKED DATA for %s :: "),*ID)+Record.Buffer);
		LOG_SV(Debug,ESeverity::Info,FString("Deserialized :: ")+ID);
	} else if (Object->IsA(AActor::StaticClass())) {
		auto Actor = CastChecked<AActor>(Object);
		if (IsInGameThread()) {DeserializeActor(Record,Actor);} else {
			FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
				FSimpleDelegateGraphTask::FDelegate::CreateStatic(&DeserializeActor,Record,Actor),
				GET_STATID(STAT_FSimpleDelegateGraphTask_DeserializeEntity),
				nullptr, ENamedThreads::GameThread
			);//
		}///
		//
		FString ID = Reflector::MakeActorID(Actor).ToString();
		LOG_SV(DeepLogs,ESeverity::Info,FString::Printf(TEXT("UNPACKED DATA for %s :: "),*ID)+Record.Buffer);
		LOG_SV(Debug,ESeverity::Info,FString("Deserialized :: ")+ID);
	} else {
		FString ID = Reflector::MakeObjectID(Object).ToString();
		LOG_SV(DeepLogs,ESeverity::Info,FString::Printf(TEXT("UNPACKED DATA for %s :: "),*ID)+Record.Buffer);
		LOG_SV(Debug,ESeverity::Info,FString("Deserialized :: ")+ID);
	}///
	//
	Result = ESaviorResult::Success;
}

void USavior3::UnpackRecord_Component(const FSaviorRecord &Record, UActorComponent* Component, ESaviorResult &Result) {
	UnpackRecord_Object(Record,Component,Result);
}

void USavior3::UnpackRecord_Actor(const FSaviorRecord &Record, AActor* Actor, ESaviorResult &Result) {
	UnpackRecord_Object(Record,Actor,Result);
}

FSaviorMinimal USavior3::GenerateMinimalRecord_Object(const UObject * Object) {
	FSaviorMinimal Record; FString Buffer;
	if (Object==nullptr) {return Record;}
	//
	TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
	TSharedRef<TJsonWriter<TCHAR,TCondensedJsonPrintPolicy<TCHAR>>>JBuffer = TJsonWriterFactory<TCHAR,TCondensedJsonPrintPolicy<TCHAR>>::Create(&Buffer);
	//
	//
	for (TFieldIterator<FProperty>PIT(Object->GetClass(),EFieldIteratorFlags::IncludeSuper); PIT; ++PIT) {
		FProperty* Property = *PIT;
		//
		const auto PID = GetMappedPropertyName(Object->GetClass(),Property->GetFName());
		if (!Property->HasAnyPropertyFlags(CPF_SaveGame)) {continue;}
		if (!IsSupportedProperty(Property)) {continue;}
		const FString ID = PID.ToString();
		//
		const bool IsSet = Property->IsA(FSetProperty::StaticClass());
		const bool IsMap = Property->IsA(FMapProperty::StaticClass());
		const bool IsInt = Property->IsA(FIntProperty::StaticClass());
		const bool IsBool = Property->IsA(FBoolProperty::StaticClass());
		const bool IsByte = Property->IsA(FByteProperty::StaticClass());
		const bool IsEnum = Property->IsA(FEnumProperty::StaticClass());
		const bool IsName = Property->IsA(FNameProperty::StaticClass());
		const bool IsText = Property->IsA(FTextProperty::StaticClass());
		const bool IsString = Property->IsA(FStrProperty::StaticClass());
		const bool IsArray = Property->IsA(FArrayProperty::StaticClass());
		const bool IsFloat = Property->IsA(FFloatProperty::StaticClass());
		const bool IsInt64 = Property->IsA(FInt64Property::StaticClass());
		const bool IsClass = Property->IsA(FClassProperty::StaticClass());
		const bool IsStruct = Property->IsA(FStructProperty::StaticClass());
		const bool IsObject = Property->IsA(FObjectProperty::StaticClass());
		//
		if (IsBool && ID.Equals(TEXT("Destroyed"),ESearchCase::IgnoreCase)) {
			Record.Destroyed = CastFieldChecked<FBoolProperty>(Property)->GetPropertyValue_InContainer(Object);
		continue;}
		//
		if (IsInt) {JSON->SetField(ID,UPropertyToJSON(CastFieldChecked<FIntProperty>(Property),Object)); continue;}
		if (IsBool) {JSON->SetField(ID,UPropertyToJSON(CastFieldChecked<FBoolProperty>(Property),Object)); continue;}
		if (IsByte) {JSON->SetField(ID,UPropertyToJSON(CastFieldChecked<FByteProperty>(Property),Object)); continue;}
		if (IsEnum) {JSON->SetField(ID,UPropertyToJSON(CastFieldChecked<FEnumProperty>(Property),Object)); continue;}
		if (IsName) {JSON->SetField(ID,UPropertyToJSON(CastFieldChecked<FNameProperty>(Property),Object)); continue;}
		if (IsText) {JSON->SetField(ID,UPropertyToJSON(CastFieldChecked<FTextProperty>(Property),Object)); continue;}
		if (IsString) {JSON->SetField(ID,UPropertyToJSON(CastFieldChecked<FStrProperty>(Property),Object)); continue;}
		if (IsFloat) {JSON->SetField(ID,UPropertyToJSON(CastFieldChecked<FFloatProperty>(Property),Object)); continue;}
		if (IsInt64) {JSON->SetField(ID,UPropertyToJSON(CastFieldChecked<FInt64Property>(Property),Object)); continue;}
		if (IsClass) {JSON->SetObjectField(ID,UPropertyToJSON(CastFieldChecked<FClassProperty>(Property),Object)); continue;}
		if (IsObject) {JSON->SetObjectField(ID,UPropertyToJSON(CastFieldChecked<FObjectProperty>(Property),Object)); continue;}
		//
		if (IsSet) {JSON->SetArrayField(ID,UPropertyToJSON(CastFieldChecked<FSetProperty>(Property),Object)); continue;}
		if (IsMap) {JSON->SetObjectField(ID,UPropertyToJSON(CastFieldChecked<FMapProperty>(Property),Object)); continue;}
		if (IsArray) {JSON->SetArrayField(ID,UPropertyToJSON(CastFieldChecked<FArrayProperty>(Property),Object)); continue;}
		//
		if ((IsStruct)&&(CastFieldChecked<FStructProperty>(Property)->Struct==TBaseStructure<FGuid>::Get())&&(Property->GetName().Equals(TEXT("SGUID"),ESearchCase::IgnoreCase))) {continue;} else
		if ((IsStruct)&&(CastFieldChecked<FStructProperty>(Property)->Struct==TBaseStructure<FTransform>::Get())) {JSON->SetObjectField(ID,UPropertyToJSON(CastFieldChecked<FStructProperty>(Property),Object,EStructType::Transform)); continue;} else
		if ((IsStruct)&&(CastFieldChecked<FStructProperty>(Property)->Struct==TBaseStructure<FLinearColor>::Get())) {JSON->SetObjectField(ID,UPropertyToJSON(CastFieldChecked<FStructProperty>(Property),Object,EStructType::LColor)); continue;} else
		if ((IsStruct)&&(CastFieldChecked<FStructProperty>(Property)->Struct==TBaseStructure<FVector2D>::Get())) {JSON->SetObjectField(ID,UPropertyToJSON(CastFieldChecked<FStructProperty>(Property),Object,EStructType::Vector2D)); continue;} else
		if ((IsStruct)&&(CastFieldChecked<FStructProperty>(Property)->Struct==FRuntimeFloatCurve::StaticStruct())) {JSON->SetObjectField(ID,UPropertyToJSON(CastFieldChecked<FStructProperty>(Property),Object,EStructType::Curve)); continue;} else
		if ((IsStruct)&&(CastFieldChecked<FStructProperty>(Property)->Struct==TBaseStructure<FRotator>::Get())) {JSON->SetObjectField(ID,UPropertyToJSON(CastFieldChecked<FStructProperty>(Property),Object,EStructType::Rotator)); continue;} else
		if ((IsStruct)&&(CastFieldChecked<FStructProperty>(Property)->Struct==TBaseStructure<FVector>::Get())) {JSON->SetObjectField(ID,UPropertyToJSON(CastFieldChecked<FStructProperty>(Property),Object,EStructType::Vector3D)); continue;} else
		if ((IsStruct)&&(CastFieldChecked<FStructProperty>(Property)->Struct==TBaseStructure<FColor>::Get())) {JSON->SetObjectField(ID,UPropertyToJSON(CastFieldChecked<FStructProperty>(Property),Object,EStructType::FColor)); continue;} else
		if ((IsStruct)) {JSON->SetObjectField(ID,UPropertyToJSON(CastFieldChecked<FStructProperty>(Property),Object,EStructType::Struct)); continue;}
	} FJsonSerializer::Serialize(JSON.ToSharedRef(),JBuffer);
	//
	//
	const auto &Settings = GetDefault<USavior3Settings>();
	//
	if (Object->IsA(UActorComponent::StaticClass())) {
		auto Component = CastChecked<UActorComponent>(Object);
		const auto &Scene = Cast<USceneComponent>(Component);
		//
		if (!TAG_CNOTRAN(Component) && (Scene && Scene->Mobility==EComponentMobility::Movable)) {
			Record.Location = Scene->GetRelativeTransform().GetLocation();
			Record.Rotation = Scene->GetRelativeTransform().GetRotation().Rotator();
		}///
		//
		if (TAG_CNOKILL(Component)) {Record.Destroyed=false;}
		if (!TAG_CNODATA(Component)) {Record.Buffer=Buffer;}
		//
		LOG_SV(Debug,ESeverity::Info,FString("(Minimal) Serialized Component :: ")+Component->GetName());
	} else if (Object->IsA(AActor::StaticClass())) {
		auto Actor = CastChecked<AActor>(Object);
		//
		if (!TAG_ANOTRAN(Actor)) {
			Record.Location = Actor->ActorToWorld().GetLocation();
			Record.Rotation = Actor->ActorToWorld().GetRotation().Rotator();
		}///
		//
		if (TAG_ANOKILL(Actor)) {Record.Destroyed=false;}
		if (!TAG_ANODATA(Actor)) {Record.Buffer=Buffer;}
		//
		LOG_SV(Debug,ESeverity::Info,FString("(Minimal) Serialized Actor :: ")+Actor->GetName());
	} else {Record.Buffer=Buffer;}
	//
	//
	LOG_SV(DeepLogs,ESeverity::Info,FString::Printf(TEXT("SAVED MINIMAL DATA for %s :: "),*Object->GetName())+Buffer);
	//
	return Record;
}

FSaviorMinimal USavior3::GenerateMinimalRecord_Component(const UActorComponent* Component) {
	return GenerateMinimalRecord_Object(Component);
}

FSaviorMinimal USavior3::GenerateMinimalRecord_Actor(const AActor* Actor) {
	return GenerateMinimalRecord_Object(Actor);
}

void USavior3::UnpackMinimalRecord_Object(const FSaviorMinimal &Record, UObject* Object, ESaviorResult &Result) {
	if (Object==nullptr||Object->IsPendingKill()) {Result=ESaviorResult::Failed; return;}
	const auto &Settings = GetDefault<USavior3Settings>();
	//
	TSharedRef<TJsonReader<TCHAR>>JReader = TJsonReaderFactory<TCHAR>::Create(Record.Buffer);
	TSharedPtr<FJsonObject>JSON = MakeShareable(new FJsonObject);
	//
	if (!FJsonSerializer::Deserialize(JReader,JSON)) {
		LOG_SV(Debug,ESeverity::Warning,FString::Printf(TEXT("[Data<->Buffer]: Corrupted Data. Unable to unpack Data Record for Object: [%s]"),*Object->GetName()));
	}///
	//
	//
	for (TFieldIterator<FProperty>PIT(Object->GetClass(),EFieldIteratorFlags::IncludeSuper); PIT; ++PIT) {
		FProperty* Property = *PIT;
		//
		const auto PID = GetMappedPropertyName(Object->GetClass(),Property->GetFName());
		if (!Property->HasAnyPropertyFlags(CPF_SaveGame)) {continue;}
		if (!IsSupportedProperty(Property)) {continue;}
		const FString ID = PID.ToString();
		//
		const bool IsSet = Property->IsA(FSetProperty::StaticClass());
		const bool IsMap = Property->IsA(FMapProperty::StaticClass());
		const bool IsInt = Property->IsA(FIntProperty::StaticClass());
		const bool IsBool = Property->IsA(FBoolProperty::StaticClass());
		const bool IsByte = Property->IsA(FByteProperty::StaticClass());
		const bool IsEnum = Property->IsA(FEnumProperty::StaticClass());
		const bool IsName = Property->IsA(FNameProperty::StaticClass());
		const bool IsText = Property->IsA(FTextProperty::StaticClass());
		const bool IsString = Property->IsA(FStrProperty::StaticClass());
		const bool IsArray = Property->IsA(FArrayProperty::StaticClass());
		const bool IsFloat = Property->IsA(FFloatProperty::StaticClass());
		const bool IsInt64 = Property->IsA(FInt64Property::StaticClass());
		const bool IsClass = Property->IsA(FClassProperty::StaticClass());
		const bool IsStruct = Property->IsA(FStructProperty::StaticClass());
		const bool IsObject = Property->IsA(FObjectProperty::StaticClass());
		//
		if ((IsInt)&&(JSON->HasField(ID))) {JSONToFProperty(JSON,ID,CastFieldChecked<FIntProperty>(Property),Object); continue;}
		if ((IsBool)&&(JSON->HasField(ID))) {JSONToFProperty(JSON,ID,CastFieldChecked<FBoolProperty>(Property),Object); continue;}
		if ((IsByte)&&(JSON->HasField(ID))) {JSONToFProperty(JSON,ID,CastFieldChecked<FByteProperty>(Property),Object); continue;}
		if ((IsEnum)&&(JSON->HasField(ID))) {JSONToFProperty(JSON,ID,CastFieldChecked<FEnumProperty>(Property),Object); continue;}
		if ((IsName)&&(JSON->HasField(ID))) {JSONToFProperty(JSON,ID,CastFieldChecked<FNameProperty>(Property),Object); continue;}
		if ((IsText)&&(JSON->HasField(ID))) {JSONToFProperty(JSON,ID,CastFieldChecked<FTextProperty>(Property),Object); continue;}
		if ((IsString)&&(JSON->HasField(ID))) {JSONToFProperty(JSON,ID,CastFieldChecked<FStrProperty>(Property),Object); continue;}
		if ((IsFloat)&&(JSON->HasField(ID))) {JSONToFProperty(JSON,ID,CastFieldChecked<FFloatProperty>(Property),Object); continue;}
		if ((IsInt64)&&(JSON->HasField(ID))) {JSONToFProperty(JSON,ID,CastFieldChecked<FInt64Property>(Property),Object); continue;}
		//
		if ((IsSet)&&(JSON->HasField(ID))) {JSONToFProperty(this,JSON,ID,CastFieldChecked<FSetProperty>(Property),Object); continue;}
		if ((IsMap)&&(JSON->HasField(ID))) {JSONToFProperty(this,JSON,ID,CastFieldChecked<FMapProperty>(Property),Object); continue;}
		if ((IsArray)&&(JSON->HasField(ID))) {JSONToFProperty(this,JSON,ID,CastFieldChecked<FArrayProperty>(Property),Object); continue;}
		if ((IsClass)&&(JSON->HasField(ID))) {JSONToFProperty(this,JSON,ID,CastFieldChecked<FClassProperty>(Property),Object); continue;}
		if ((IsObject)&&(JSON->HasField(ID))) {JSONToFProperty(this,JSON,ID,CastFieldChecked<FObjectProperty>(Property),Object); continue;}
		//
		if ((IsStruct)&&(CastFieldChecked<FStructProperty>(Property)->Struct==TBaseStructure<FGuid>::Get())&&(Property->GetName().Equals(TEXT("SGUID"),ESearchCase::IgnoreCase))) {continue;} else
		if ((IsStruct)&&(JSON->HasField(ID))&&(CastFieldChecked<FStructProperty>(Property)->Struct==TBaseStructure<FTransform>::Get())) {JSONToFProperty(JSON,ID,CastFieldChecked<FStructProperty>(Property),Object,EStructType::Transform); continue;} else
		if ((IsStruct)&&(JSON->HasField(ID))&&(CastFieldChecked<FStructProperty>(Property)->Struct==TBaseStructure<FLinearColor>::Get())) {JSONToFProperty(JSON,ID,CastFieldChecked<FStructProperty>(Property),Object,EStructType::LColor); continue;} else
		if ((IsStruct)&&(JSON->HasField(ID))&&(CastFieldChecked<FStructProperty>(Property)->Struct==TBaseStructure<FVector2D>::Get())) {JSONToFProperty(JSON,ID,CastFieldChecked<FStructProperty>(Property),Object,EStructType::Vector2D); continue;} else
		if ((IsStruct)&&(JSON->HasField(ID))&&(CastFieldChecked<FStructProperty>(Property)->Struct==FRuntimeFloatCurve::StaticStruct())) {JSONToFProperty(JSON,ID,CastFieldChecked<FStructProperty>(Property),Object,EStructType::Curve); continue;} else
		if ((IsStruct)&&(JSON->HasField(ID))&&(CastFieldChecked<FStructProperty>(Property)->Struct==TBaseStructure<FRotator>::Get())) {JSONToFProperty(JSON,ID,CastFieldChecked<FStructProperty>(Property),Object,EStructType::Rotator); continue;} else
		if ((IsStruct)&&(JSON->HasField(ID))&&(CastFieldChecked<FStructProperty>(Property)->Struct==TBaseStructure<FVector>::Get())) {JSONToFProperty(JSON,ID,CastFieldChecked<FStructProperty>(Property),Object,EStructType::Vector3D); continue;} else
		if ((IsStruct)&&(JSON->HasField(ID))&&(CastFieldChecked<FStructProperty>(Property)->Struct==TBaseStructure<FColor>::Get())) {JSONToFProperty(JSON,ID,CastFieldChecked<FStructProperty>(Property),Object,EStructType::FColor); continue;} else
		if ((IsStruct)&&(JSON->HasField(ID))) {JSONToFProperty(JSON,ID,CastFieldChecked<FStructProperty>(Property),Object,EStructType::Struct); continue;}
	}///
	//
	//
	if (Object->IsA(UActorComponent::StaticClass())) {
		auto Component = CastChecked<UActorComponent>(Object);
		if (IsInGameThread()) {DeserializeComponent(Record,Component);} else {
			FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
				FSimpleDelegateGraphTask::FDelegate::CreateStatic(&DeserializeComponent,Record,Component),
				GET_STATID(STAT_FSimpleDelegateGraphTask_DeserializeEntity),
				nullptr, ENamedThreads::GameThread
			);//
		}///
	} else if (Object->IsA(AActor::StaticClass())) {
		auto Actor = CastChecked<AActor>(Object);
		if (IsInGameThread()) {DeserializeActor(Record,Actor);} else {
			FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(
				FSimpleDelegateGraphTask::FDelegate::CreateStatic(&DeserializeActor,Record,Actor),
				GET_STATID(STAT_FSimpleDelegateGraphTask_DeserializeEntity),
				nullptr, ENamedThreads::GameThread
			);//
		}///
	}///
	//
	//
	LOG_SV(DeepLogs,ESeverity::Info,FString::Printf(TEXT("UNPACKED MINIMAL DATA for %s :: "),*Object->GetName())+Record.Buffer);
	LOG_SV(Debug,ESeverity::Info,FString("Deserialized :: ")+Object->GetName());
	//
	Result = ESaviorResult::Success;
}

void USavior3::UnpackMinimalRecord_Component(const FSaviorMinimal &Record, UActorComponent* Component, ESaviorResult &Result) {
	UnpackMinimalRecord_Object(Record,Component,Result);
}

void USavior3::UnpackMinimalRecord_Actor(const FSaviorMinimal &Record, AActor* Actor, ESaviorResult &Result) {
	UnpackMinimalRecord_Object(Record,Actor,Result);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Savior Utilities:

USavior3* USavior3::NewSlotInstance(UObject* Context, USavior3* Slot, ESaviorResult &Result) {
	if (Context==nullptr||Context->GetWorld()==nullptr||Context->IsPendingKill()) {Result=ESaviorResult::Failed; return nullptr;}
	if (Slot==nullptr) {Result=ESaviorResult::Failed; return nullptr;}
	//
	auto Instance = NewObject<USavior3>(Context->GetWorld(),Slot->GetClass(),Slot->GetFName(),RF_NoFlags,Slot);
	Instance->SetWorld(Context->GetWorld());
	//
	if (Instance->GetSlotData()==nullptr) {
		const auto Data = NewObject<USlotData>(Instance,TEXT("SlotData"));
		Instance->SetSlotData(Data);
	} if (Instance->GetSlotMeta()==nullptr) {
		const auto Meta = NewObject<USlotMeta>(Instance,TEXT("SlotMeta"));
		Instance->SetSlotMeta(Meta);
	} Result = ESaviorResult::Success;
	//
	return Instance;
}

UObject* USavior3::GetClassDefaultObject(UClass* Class) {
	return Class->GetDefaultObject(true);
}

UObject* USavior3::NewObjectInstance(UObject* Context, UClass* Class) {
	if (!Context||!Context->IsValidLowLevelFast()||Context->IsPendingKill()) {return nullptr;}
	if (Class==nullptr) {return nullptr;}
	//
	auto World = GEngine->GetWorldFromContextObject(Context,EGetWorldErrorMode::LogAndReturnNull);
	if (World!=nullptr) {return NewObject<UObject>(Context,Class);}
	//
	return nullptr;
}

UObject* USavior3::NewNamedObjectInstance(UObject* Context, UClass* Class, FName Name) {
	if (!Context||!Context->IsValidLowLevelFast()||Context->IsPendingKill()) {return nullptr;}
	if (Class==nullptr) {return nullptr;}
	//
	auto World = GEngine->GetWorldFromContextObject(Context,EGetWorldErrorMode::LogAndReturnNull);
	if (World!=nullptr) {return NewObject<UObject>(Context,Class,Name);}
	//
	return nullptr;
}

FGuid USavior3::NewObjectGUID() {
	return FGuid::NewGuid();
}

FGuid USavior3::CreateSGUID(UObject* Context) {
	FGuid GUID = Reflector::FindGUID(Context);
	if (!GUID.IsValid()) {GUID=FGuid::NewGuid();}
	//
	return GUID;
}

bool USavior3::MatchesGUID(UObject* Context, UObject* ComparedTo) {
	return Reflector::IsMatchingSGUID(Context,ComparedTo);
}

AActor* USavior3::FindActorWithGUID(UObject* Context, const FGuid &SGUID) {
	if (Context->GetWorld()==nullptr) {return nullptr;}
	//
	UWorld* World = Context->GetWorld();
	for (TActorIterator<AActor>Actor(World); Actor; ++Actor) {
		if (Actor->HasAnyFlags(RF_BeginDestroyed)) {continue;}
		//
		const FGuid GUID = Reflector::FindGUID(*Actor);
		if (GUID==SGUID) {return (*Actor);}
	}///
	//
	return nullptr;
}

void USavior3::ClearWorkload() {
	USavior3::SS_Workload=0; USavior3::SS_Complete=0; USavior3::SS_Progress=100.f;
	USavior3::SL_Workload=0; USavior3::SL_Complete=0; USavior3::SL_Progress=100.f;
	//
	StaticSaviorSlot.Location = NAME_None;
	StaticSaviorSlot.Slot = NAME_None;
	//
	//
	for (TActorIterator<AActor>Actor(World); Actor; ++Actor) {
		if (!(*Actor)->GetOutermost()->ContainsMap()) {continue;}
		if (!(*Actor)->IsValidLowLevelFast()||(*Actor)->IsPendingKill()) {continue;}
		if ((*Actor)->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject|RF_BeginDestroyed)) {continue;}
		//
		if (IsActorMarkedAutoDestroy(*Actor)) {
			(*Actor)->SetActorTickEnabled(false);
			(*Actor)->SetActorHiddenInGame(true);
			(*Actor)->SetActorEnableCollision(false);
			(*Actor)->Destroy();
		}///
	}///
	//
	for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
		if ((*OBJ)->IsA(AActor::StaticClass())) {continue;}
		if ((*OBJ)->IsA(UActorComponent::StaticClass())) {continue;}
		//
		if (!(*OBJ)->GetOutermost()->ContainsMap()) {continue;}
		if (!(*OBJ)->IsValidLowLevelFast()||(*OBJ)->IsPendingKill()) {continue;}
		if ((*OBJ)->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject|RF_BeginDestroyed)) {continue;}
		//
		if (IsObjectMarkedAutoDestroy(*OBJ)) {
			(*OBJ)->BeginDestroy();
		}///
	}///
	//
	//
	USavior3::ThreadSafety = EThreadSafety::IsCurrentlyThreadSafe;
	USavior3::LastThreadState = EThreadSafety::IsCurrentlyThreadSafe;
	//
	if (World) {
		static FTimerDelegate TimerDelegate = FTimerDelegate::CreateStatic(&StaticRemoveLoadScreen);
		World->GetTimerManager().SetTimer(USavior3::TH_LoadScreen,TimerDelegate,LoadScreenTimer,false);
	} else {RemoveLoadScreen();}
}

float USavior3::CalculateWorkload() const {
	float Workload=0.f;
	//
	for (TObjectIterator<UObject>OBJ; OBJ; ++OBJ) {
		if (!(*OBJ)->GetOutermost()->ContainsMap()) {continue;}
		if (!(*OBJ)->IsValidLowLevelFast()||(*OBJ)->IsPendingKill()) {continue;}
		if ((*OBJ)->HasAnyFlags(RF_ClassDefaultObject|RF_ArchetypeObject|RF_BeginDestroyed)) {continue;}
		//
		if ((*OBJ)->IsA(AActor::StaticClass())) {
			for (auto ACT : ActorScope) {
				if (ACT.Get()==nullptr) {continue;}
				if ((*OBJ)->IsA(ACT)) {Workload++; break;}
			}///
		} else if ((*OBJ)->IsA(UActorComponent::StaticClass())) {
			for (auto COM : ComponentScope) {
				if (COM.Get()==nullptr) {continue;}
				if ((*OBJ)->IsA(COM)) {Workload++; break;}
			}///
		} else {
			for (auto OBS : ObjectScope) {
				if (OBS.Get()==nullptr) {continue;}
				if ((*OBJ)->IsA(OBS)) {Workload++; break;}
			}///
		}///
	}///
	//
	return Workload;
}

FName USavior3::GetMappedPropertyName(TSubclassOf<UObject> Class, const FName Property) const {
	auto Map = Redirectors.Find(Class);
	//
	if (Map!=nullptr) {
		auto Redirect = (*Map).PropertyRedirect;
		if (Redirect.Contains(Property)) {
			return *Redirect.Find(Property);
		}///
	}///
	//
	return Property;
}

ESaviorResult USavior3::MarkObjectAutoDestroyed(UObject* Object) {
	if ((Object!=nullptr)&&(!Object->IsPendingKill())) {
		bool HasProperty = false;
		//
		for (TFieldIterator<FProperty>PIT(Object->GetClass(),EFieldIteratorFlags::IncludeSuper); PIT; ++PIT) {
			FProperty* Property = *PIT;
			const bool IsBool = Property->IsA(FBoolProperty::StaticClass());
			const auto PID = Property->GetFName();
			if (IsBool && PID.ToString().Equals(TEXT("Destroyed"),ESearchCase::IgnoreCase)) {
				auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Object);
				CastFieldChecked<FBoolProperty>(Property)->SetPropertyValue(ValuePtr,true);
				HasProperty = true;
		break;}}
		//
		if (!HasProperty) {
			LOG_SV(true,ESeverity::Warning,FString::Printf(TEXT("'Auto-Destroy' function requires Target Object to have a Boolean Property named 'Destroyed'. Property wasn't found in this Class:  %s"),*Object->GetClass()->GetName()));
		return ESaviorResult::Failed;}
		//
		const auto &Interface = Cast<ISAVIOR_Serializable>(Object);
		if (Interface) {Interface->Execute_OnMarkedAutoDestroy(Object);}
		else if (Object->GetClass()->ImplementsInterface(USAVIOR_Serializable::StaticClass())) {
			ISAVIOR_Serializable::Execute_OnMarkedAutoDestroy(Object);
		}///
	} return ESaviorResult::Success;
}

bool USavior3::IsObjectMarkedAutoDestroy(UObject* Object) {
	if ((Object!=nullptr)&&(!Object->IsPendingKill())) {
		bool IsMarked = false;
		//
		for (TFieldIterator<FProperty>PIT(Object->GetClass(),EFieldIteratorFlags::IncludeSuper); PIT; ++PIT) {
			FProperty* Property = *PIT;
			const bool IsBool = Property->IsA(FBoolProperty::StaticClass());
			const auto PID = Property->GetFName();
			//
			if (IsBool && PID.ToString().Equals(TEXT("Destroyed"),ESearchCase::IgnoreCase)) {
				const auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Object);
				IsMarked = CastFieldChecked<FBoolProperty>(Property)->GetPropertyValue(ValuePtr);
		break;}}
		//
		return IsMarked;
	} return false;
}

ESaviorResult USavior3::MarkComponentAutoDestroyed(UActorComponent* Component) {
	if ((Component!=nullptr)&&(!Component->IsPendingKill())) {
		bool HasProperty = false;
		//
		for (TFieldIterator<FProperty>PIT(Component->GetClass(),EFieldIteratorFlags::IncludeSuper); PIT; ++PIT) {
			FProperty* Property = *PIT;
			const bool IsBool = Property->IsA(FBoolProperty::StaticClass());
			const auto PID = Property->GetFName();
			if (IsBool && PID.ToString().Equals(TEXT("Destroyed"),ESearchCase::IgnoreCase)) {
				auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Component);
				CastFieldChecked<FBoolProperty>(Property)->SetPropertyValue(ValuePtr,true);
				HasProperty = true;
		break;}}
		//
		if (!HasProperty) {
			LOG_SV(true,ESeverity::Warning,FString::Printf(TEXT("'Auto-Destroy' function requires Target Component to have a Boolean Property named 'Destroyed'. Property wasn't found in this Class:  %s"),*Component->GetClass()->GetName()));
		return ESaviorResult::Failed;}
		//
		const auto &Interface = Cast<ISAVIOR_Serializable>(Component);
		if (Interface) {Interface->Execute_OnMarkedAutoDestroy(Component);}
		else if (Component->GetClass()->ImplementsInterface(USAVIOR_Serializable::StaticClass())) {
			ISAVIOR_Serializable::Execute_OnMarkedAutoDestroy(Component);
		}///
	} return ESaviorResult::Success;
}

bool USavior3::IsComponentMarkedAutoDestroy(UActorComponent* Component) {
	if ((Component!=nullptr)&&(!Component->IsPendingKill())) {
		bool IsMarked = false;
		//
		for (TFieldIterator<FProperty>PIT(Component->GetClass(),EFieldIteratorFlags::IncludeSuper); PIT; ++PIT) {
			FProperty* Property = *PIT;
			const bool IsBool = Property->IsA(FBoolProperty::StaticClass());
			const auto PID = Property->GetFName();
			//
			if (IsBool && PID.ToString().Equals(TEXT("Destroyed"),ESearchCase::IgnoreCase)) {
				const auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Component);
				IsMarked = CastFieldChecked<FBoolProperty>(Property)->GetPropertyValue(ValuePtr);
		break;}}
		//
		return IsMarked;
	} return false;
}

ESaviorResult USavior3::MarkActorAutoDestroyed(AActor* Actor) {
	if ((Actor!=nullptr)&&(!Actor->IsPendingKill())) {
		bool HasProperty = false;
		//
		for (TFieldIterator<FProperty>PIT(Actor->GetClass(),EFieldIteratorFlags::IncludeSuper); PIT; ++PIT) {
			FProperty* Property = *PIT;
			const bool IsBool = Property->IsA(FBoolProperty::StaticClass());
			const auto PID = Property->GetFName();
			if (IsBool && PID.ToString().Equals(TEXT("Destroyed"),ESearchCase::IgnoreCase)) {
				auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Actor);
				CastFieldChecked<FBoolProperty>(Property)->SetPropertyValue(ValuePtr,true);
				HasProperty = true;
		break;}}
		//
		if (!HasProperty) {
			LOG_SV(true,ESeverity::Warning,FString::Printf(TEXT("'Auto-Destroy' function requires Target Actor to have a Boolean Property named 'Destroyed'. Property wasn't found in this Class:  %s"),*Actor->GetClass()->GetName()));
		return ESaviorResult::Failed;}
		//
		const auto &Interface = Cast<ISAVIOR_Serializable>(Actor);
		if (Interface) {Interface->Execute_OnMarkedAutoDestroy(Actor);}
		else if (Actor->GetClass()->ImplementsInterface(USAVIOR_Serializable::StaticClass())) {
			ISAVIOR_Serializable::Execute_OnMarkedAutoDestroy(Actor);
		}///
	} return ESaviorResult::Success;
}

bool USavior3::IsActorMarkedAutoDestroy(AActor* Actor) {
	if ((Actor!=nullptr)&&(!Actor->IsPendingKill())) {
		bool IsMarked = false;
		//
		for (TFieldIterator<FProperty>PIT(Actor->GetClass(),EFieldIteratorFlags::IncludeSuper); PIT; ++PIT) {
			FProperty* Property = *PIT;
			const bool IsBool = Property->IsA(FBoolProperty::StaticClass());
			const auto PID = Property->GetFName();
			//
			if (IsBool && PID.ToString().Equals(TEXT("Destroyed"),ESearchCase::IgnoreCase)) {
				const auto ValuePtr = Property->ContainerPtrToValuePtr<void>(Actor);
				IsMarked = CastFieldChecked<FBoolProperty>(Property)->GetPropertyValue(ValuePtr);
		break;}}
		//
		return IsMarked;
	} return false;
}

void USavior3::SetDefaultPlayerID(const int32 NewID) {
	const auto &Settings = GetMutableDefault<USavior3Settings>();
	//
	const uint32 ID = (NewID < 0) ? 0 : NewID;
	if (Settings!=nullptr) {Settings->DefaultPlayerID=ID;}
}

void USavior3::SetDefaultPlayerLevel(const int32 NewLevel) {
	const auto &Settings = GetMutableDefault<USavior3Settings>();
	//
	const uint32 Level = (NewLevel < 0) ? 0 : NewLevel;
	if (Settings!=nullptr) {Settings->DefaultPlayerID=Level;}
}

void USavior3::SetDefaultPlayerName(const FString NewName) {
	const auto &Settings = GetMutableDefault<USavior3Settings>();
	if (Settings!=nullptr) {Settings->DefaultPlayerName=NewName;}
}

void USavior3::SetDefaultChapter(const FString NewChapter) {
	const auto &Settings = GetMutableDefault<USavior3Settings>();
	if (Settings!=nullptr) {Settings->DefaultChapter=NewChapter;}
}

void USavior3::SetDefaultLocation(const FString NewLocation) {
	const auto &Settings = GetMutableDefault<USavior3Settings>();
	if (Settings!=nullptr) {Settings->DefaultLocation=NewLocation;}
}

UTexture2D* USavior3::GetSlotThumbnail(FVector2D &TextureSize) {
	FString Name;
	for (auto Level : LevelThumbnails) {
		if (Level.Value.IsNull()) {continue;}
		Level.Key.ToString().Split(TEXT("."),nullptr,&Name,ESearchCase::IgnoreCase,ESearchDir::FromEnd);
		//
		if (Name==SlotMeta->SaveLocation) {
			SlotThumbnail = Level.Value.ToSoftObjectPath();
		break;}
	}///
	//
	auto OBJ = SlotThumbnail.TryLoad();
	if (UTexture2D* Image = Cast<UTexture2D>(OBJ)) {
		TextureSize = FVector2D(Image->GetSizeX(),Image->GetSizeY());
		return Image;
	}///
	//
	return nullptr;
}

FString USavior3::GetSaveTimeISO() {
	FString S;
	//
	SlotMeta->SaveDate.ToIso8601().Split("T",nullptr,&S);
	S.Split(".",&S,nullptr);
	//
	return S;
}

FString USavior3::GetSaveDateISO() {
	FString S;
	if (SlotMeta==nullptr) {return FString();}
	SlotMeta->SaveDate.ToIso8601().Split("T",&S,nullptr);
	//
	return S;
}

USlotMeta* USavior3::GetSlotMetaData() const {
	return SlotMeta;
}

TMap<FName,FSaviorRecord> USavior3::GetSlotDataCopy() {
	if (SlotData==nullptr) {return TMap<FName,FSaviorRecord>();}
	//
	return SlotData->Complex;
}

bool ParseActorTAG(const AActor* Actor, const FName Tag) {
	return Actor->ActorHasTag(Tag);
}

bool ParseComponentTAG(const UActorComponent* Component, const FName Tag) {
	return Component->ComponentHasTag(Tag);
}

void USavior3::SetProgress(float New) {
	if (SlotMeta==nullptr) {return;}
	//
	SlotMeta->Progress = New;
}

void USavior3::SetPlayTime(int32 New) {
	if (SlotMeta==nullptr) {return;}
	//
	SlotMeta->PlayTime = New;
}

void USavior3::SetChapter(FString New) {
	if (SlotMeta==nullptr) {return;}
	//
	SlotMeta->Chapter = New;
}

void USavior3::SetPlayerLevel(int32 New) {
	if (SlotMeta==nullptr) {return;}
	//
	SlotMeta->PlayerLevel = New;
}

void USavior3::SetSaveDate(FDateTime New) {
	if (SlotMeta==nullptr) {return;}
	//
	SlotMeta->SaveDate = New;
}

void USavior3::SetPlayerName(FString New) {
	if (SlotMeta==nullptr) {return;}
	//
	SlotMeta->PlayerName = New;
}

void USavior3::SetSaveLocation(FString New) {
	if (SlotMeta==nullptr) {return;}
	//
	SlotMeta->SaveLocation = New;
}

float USavior3::GetProgress() {
	if (SlotMeta==nullptr) {return 0.f;}
	//
	return SlotMeta->Progress;
}

int32 USavior3::GetPlayTime() {
	if (SlotMeta==nullptr) {return 0;}
	//
	return SlotMeta->PlayTime;
}

FString USavior3::GetChapter() {
	if (SlotMeta==nullptr) {return FString();}
	//
	return SlotMeta->Chapter;
}

int32 USavior3::GetPlayerLevel() {
	if (SlotMeta==nullptr) {return 0;}
	//
	return SlotMeta->PlayerLevel;
}

FDateTime USavior3::GetSaveDate() {
	if (SlotMeta==nullptr) {return FDateTime();}
	//
	return SlotMeta->SaveDate;
}

FString USavior3::GetPlayerName() {
	if (SlotMeta==nullptr) {return FString();}
	//
	return SlotMeta->PlayerName;
}

FString USavior3::GetSaveLocation() {
	if (SlotMeta==nullptr) {return FString();}
	//
	return SlotMeta->SaveLocation;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Savior Loading-Screen Interface:

void USavior3::LaunchLoadScreen(const EThreadSafety Mode, const FText Info) {
	if (GEngine==nullptr||World==nullptr) {return;}
	const auto &PC = World->GetFirstPlayerController();
	//
	if (PC!=nullptr) {
		const auto &HUD = Cast<AHUD_SaviorUI>(PC->GetHUD());
		if (HUD!=nullptr) {
			switch (LoadScreenMode) {
				case ELoadScreenMode::BackgroundBlur:
				{
					HUD->DisplayBlurLoadScreenHUD(Mode,Info,FeedbackFont,ProgressBarTint,BackBlurPower);
				}	break;
				//
				case ELoadScreenMode::SplashScreen:
				{
					HUD->DisplaySplashLoadScreenHUD(Mode,Info,FeedbackFont,ProgressBarTint,SplashImage,SplashStretch);
				}	break;
				//
				case ELoadScreenMode::MoviePlayer:
				{
					HUD->DisplayMovieLoadScreenHUD(Mode,Info,FeedbackFont,ProgressBarTint,SplashMovie,ProgressBarOnMovie);
				}	break;
			default: break;}
		}///
	}///
}

void USavior3::RemoveLoadScreen() {
	StaticRemoveLoadScreen();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////