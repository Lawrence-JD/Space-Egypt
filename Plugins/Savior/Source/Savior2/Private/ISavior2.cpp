//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//			Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ISavior2.h"

#include "Savior2.h"
#include "Savior2_Shared.h"
#include "LoadScreen/HUD_LoadScreenStyle.h"

#if WITH_EDITORONLY_DATA
 #include "ISettingsModule.h"
 #include "ISettingsSection.h"
 #include "ISettingsContainer.h"
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define LOCTEXT_NAMESPACE "Synaptech"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class FSavior2 : public ISavior2 {
private:
	bool HandleSettingsSaved() {
	  #if WITH_EDITORONLY_DATA
		const auto &Settings = GetMutableDefault<USavior2Settings>();
		Settings->SaveConfig(); return true;
	  #endif
	return false;}
	//
	void RegisterSettings() {
	  #if WITH_EDITORONLY_DATA
		if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings")) {
			ISettingsContainerPtr SettingsContainer = SettingsModule->GetContainer("Project");
			SettingsContainer->DescribeCategory("Synaptech",LOCTEXT("SynaptechCategoryName","Synaptech"),
			LOCTEXT("SynaptechCategoryDescription","Configuration of Synaptech Systems."));
			//
			ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project","Synaptech","Savior2Settings",
				LOCTEXT("Savior2SettingsName","Savior 2 Settings"),
				LOCTEXT("Savior2SettingsDescription","General settings for the Savior 2 Plugin"),
			GetMutableDefault<USavior2Settings>());
			//
			if (SettingsSection.IsValid()) {SettingsSection->OnModified().BindRaw(this,&FSavior2::HandleSettingsSaved);}
		}///
	  #endif
	}
	//
	void UnregisterSettings() {
	  #if WITH_EDITORONLY_DATA
		if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings")) {
			SettingsModule->UnregisterSettings("Project","Synaptech","Savior2Settings");
		}///
	  #endif
	}
public:
	virtual void StartupModule() override {
		RegisterSettings();
		FSaviorLoadScreenStyle::Initialize();
		//
		const auto VER = FString(TEXT("{ Savior 2 } Initializing Plugin:  v"))+PLUGIN_VERSION;
		LOG_SV(true,ESeverity::Warning,VER);
	}///
	//
	virtual void ShutdownModule() override {
		FSaviorLoadScreenStyle::Shutdown();
		if (UObjectInitialized()) {UnregisterSettings();}
	}///
	//
    virtual bool SupportsDynamicReloading() override {return true;}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
IMPLEMENT_GAME_MODULE(FSavior2,Savior2);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////