//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Savior2.h"
#include "SlotScreen/HUD_SlotPicker.h"
#include "Savior2_Shared.h"

#include "Runtime/MoviePlayer/Public/MoviePlayer.h"
#include "Runtime/Engine/Classes/GameFramework/HUD.h"

#include "HUD_SaveLoadScreen.generated.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class USavior2;
class HUD_LoadScreenBlurWidget;
class HUD_LoadScreenMovieWidget;
class HUD_LoadScreenSplashWidget;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHUD_LoadScreen);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UCLASS(ClassGroup=Synaptech, Category="Synaptech", Blueprintable, BlueprintType, meta=(DisplayName="[SAVIOR] HUD"))
class SAVIOR2_API AHUD_SaviorUI : public AHUD {
	GENERATED_BODY()
	AHUD_SaviorUI();
	//
	friend class USavior2;
	friend class HUD_LoadScreenBlurWidget;
	friend class HUD_LoadScreenMovieWidget;
	friend class HUD_LoadScreenSplashWidget;
private:
	FDelegateHandle OnFinishMovie;
	UPROPERTY() USlotPickerWidget* SlotPickerUI;
protected:
	TSharedPtr<HUD_LoadScreenBlurWidget> LoadScreenBlurWidget;
	TSharedPtr<HUD_LoadScreenMovieWidget> LoadScreenMovieWidget;
	TSharedPtr<HUD_LoadScreenSplashWidget> LoadScreenSplashWidget;
	//
	void DisplayBlurLoadScreenHUD(const EThreadSafety TaskMode, const FText FeedbackText, const FSlateFontInfo &FeedbackFont, const FLinearColor &ProgressBarTint, const float BlurPower);
	void RemoveBlurLoadScreenHUD();
	//
	void DisplaySplashLoadScreenHUD(const EThreadSafety TaskMode, const FText FeedbackText, const FSlateFontInfo &FeedbackFont, const FLinearColor &ProgressBarTint, const FStringAssetReference SplashAsset, TEnumAsByte<EStretch::Type> SplashStretch);
	void RemoveSplashLoadScreenHUD();
	//
	void DisplayMovieLoadScreenHUD(const EThreadSafety TaskMode, const FText FeedbackText, const FSlateFontInfo &FeedbackFont, const FLinearColor &ProgressBarTint, const FStringAssetReference SplashMovie, const bool ProgressBarOnMovie);
	void RemoveMovieLoadScreenHUD();
	void BreakLoadScreenMovie();
public:
	/// :: PROPERTIES ::
	///
	/** Widget used as Slot Picker. */
	UPROPERTY(Category="Savior 2", EditAnywhere, BlueprintReadWrite, meta=(DisplayName="UI Slot Picker"))
	TSubclassOf<USlotPickerWidget> SlotPickerWidget;
	//
	//
	/** Event fired when a Load-Screen is created. */
	UPROPERTY(Category="Savior 2", BlueprintAssignable, meta=(DisplayName="[SAVIOR] On Begin Load-Screen"))
	FHUD_LoadScreen OnBeginLoadScreen;
	//
	/** Event fired when a Load-Screen is destroyed. */
	UPROPERTY(Category="Savior 2", BlueprintAssignable, meta=(DisplayName="[SAVIOR] On Finish Load-Screen"))
	FHUD_LoadScreen OnFinishLoadScreen;
	///
	/// :: FUNCTIONS ::
	///
	/** Event called when a Load-Screen is shown. */
	UFUNCTION(Category="Savior 2", BlueprintImplementableEvent, meta=(DisplayName="[SAVIOR] On Begin Load-Screen"))
	void OnBeganLoadScreen();
	//
	/** Event called when a Load-Screen is hidden. */
	UFUNCTION(Category="Savior 2", BlueprintImplementableEvent, meta=(DisplayName="[SAVIOR] On Finish Load-Screen"))
	void OnFinishedLoadScreen();
	//
	//
	/** Adds an auto-generated Save Slot Picker UI to the Viewport. */
	UFUNCTION(Category="Savior 2", BlueprintNativeEvent, BlueprintCallable, meta=(DisplayName="[SAVIOR] Show Slots UI"))
	void ShowSlotPickerHUD(const ESlotPickerMode PickerMode);
	//
	/** Removes auto-generated Slot Picker UI from the Viewport. */
	UFUNCTION(Category="Savior 2", BlueprintNativeEvent, BlueprintCallable, meta=(DisplayName="[SAVIOR] Hide Slots UI"))
	void HideSlotPickerHUD();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////