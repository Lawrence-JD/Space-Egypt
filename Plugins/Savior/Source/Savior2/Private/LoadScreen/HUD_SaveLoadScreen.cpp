//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LoadScreen/HUD_SaveLoadScreen.h"
#include "HUD_LoadScreenSplashWidget.h"
#include "HUD_LoadScreenMovieWidget.h"
#include "HUD_LoadScreenBlurWidget.h"

#include "Runtime/Slate/Public/Widgets/SWeakWidget.h"
#include "Runtime/Engine/Classes/Engine/GameViewportClient.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// HUD Load-Screen Constructors:

AHUD_SaviorUI::AHUD_SaviorUI() {}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Load-Screen Interface:

void AHUD_SaviorUI::DisplayBlurLoadScreenHUD(const EThreadSafety TaskMode, const FText FeedbackText, const FSlateFontInfo &FeedbackFont, const FLinearColor &ProgressBarTint, const float BlurPower) {
	if (GEngine==nullptr||GEngine->GameViewport==nullptr) {return;}
	//
	if (!LoadScreenBlurWidget.IsValid()) {
		SAssignNew(LoadScreenBlurWidget,HUD_LoadScreenBlurWidget)
		.ProgressBarTint(ProgressBarTint)
		.FeedbackFont(FeedbackFont)
		.FeedbackText(FeedbackText)
		.TaskMode(TaskMode)
		.Blur(BlurPower);
	}///
	//
	if (LoadScreenBlurWidget.IsValid()) {
		GEngine->GameViewport->AddViewportWidgetContent(LoadScreenBlurWidget.ToSharedRef());
		LoadScreenBlurWidget->SetVisibility(EVisibility::Visible);
	}///
	//
	OnBeganLoadScreen();
	OnBeginLoadScreen.Broadcast();
}

void AHUD_SaviorUI::RemoveBlurLoadScreenHUD() {
	if (GEngine==nullptr||GEngine->GameViewport==nullptr) {return;}
	//
	if (LoadScreenBlurWidget.IsValid()) {
		LoadScreenBlurWidget->SetVisibility(EVisibility::Hidden);
	}///
	//
	OnFinishedLoadScreen();
	OnFinishLoadScreen.Broadcast();
}

void AHUD_SaviorUI::DisplaySplashLoadScreenHUD(const EThreadSafety TaskMode, const FText FeedbackText, const FSlateFontInfo &FeedbackFont, const FLinearColor &ProgressBarTint, const FStringAssetReference SplashAsset, TEnumAsByte<EStretch::Type> SplashStretch) {
	if (GEngine==nullptr||GEngine->GameViewport==nullptr) {return;}
	//
	if (!LoadScreenSplashWidget.IsValid()) {
		SAssignNew(LoadScreenSplashWidget,HUD_LoadScreenSplashWidget)
		.ProgressBarTint(ProgressBarTint)
		.SplashStretch(SplashStretch)
		.FeedbackFont(FeedbackFont)
		.FeedbackText(FeedbackText)
		.SplashAsset(SplashAsset)
		.TaskMode(TaskMode);
	}///
	//
	if (LoadScreenSplashWidget.IsValid()) {
		GEngine->GameViewport->AddViewportWidgetContent(LoadScreenSplashWidget.ToSharedRef());
		LoadScreenSplashWidget->SetVisibility(EVisibility::Visible);
	}///
	//
	OnBeganLoadScreen();
	OnBeginLoadScreen.Broadcast();
}

void AHUD_SaviorUI::RemoveSplashLoadScreenHUD() {
	if (GEngine==nullptr||GEngine->GameViewport==nullptr) {return;}
	//
	if (LoadScreenSplashWidget.IsValid()) {
		LoadScreenSplashWidget->SetVisibility(EVisibility::Hidden);
	}///
	//
	OnFinishedLoadScreen();
	OnFinishLoadScreen.Broadcast();
}

void AHUD_SaviorUI::DisplayMovieLoadScreenHUD(const EThreadSafety TaskMode, const FText FeedbackText, const FSlateFontInfo &FeedbackFont, const FLinearColor &ProgressBarTint, const FStringAssetReference SplashMovie, const bool ProgressBarOnMovie) {
	if (GEngine==nullptr||GEngine->GameViewport==nullptr) {return;}
	if (GetMoviePlayer()->IsMovieCurrentlyPlaying()) {return;}
	//
	auto MoviePath = SplashMovie.ToString();
	MoviePath.Split(TEXT("/Movies/"),nullptr,&MoviePath);
	MoviePath.Split(TEXT("."),&MoviePath,nullptr);
	//
	FLoadingScreenAttributes Attributes;
	Attributes.MoviePaths.Add(MoviePath);
	Attributes.bWaitForManualStop = false;
	Attributes.bMoviesAreSkippable = false;
	Attributes.bAllowInEarlyStartup = false;
	Attributes.bAutoCompleteWhenLoadingCompletes = false;
	Attributes.PlaybackType = EMoviePlaybackType::MT_Normal;
	//
	//
	if (ProgressBarOnMovie) {
		if (!LoadScreenMovieWidget.IsValid()) {
			SAssignNew(LoadScreenMovieWidget,HUD_LoadScreenMovieWidget)
			.ProgressBarTint(ProgressBarTint)
			.FeedbackFont(FeedbackFont)
			.FeedbackText(FeedbackText)
			.TaskMode(TaskMode);
		}///
		//
		if (LoadScreenMovieWidget.IsValid()) {
			LoadScreenMovieWidget->SetVisibility(EVisibility::Visible);
			Attributes.WidgetLoadingScreen = LoadScreenMovieWidget;
		}///
	}///
	//
	GetMoviePlayer()->SetupLoadingScreen(Attributes);
	if (GetMoviePlayer()->LoadingScreenIsPrepared()) {
		GetMoviePlayer()->PlayMovie();
	OnFinishMovie = GetMoviePlayer()->OnMoviePlaybackFinished().AddUObject(this,&AHUD_SaviorUI::BreakLoadScreenMovie);}
	//
	OnBeganLoadScreen();
	OnBeginLoadScreen.Broadcast();
}

void AHUD_SaviorUI::RemoveMovieLoadScreenHUD() {
	if (GEngine==nullptr||GEngine->GameViewport==nullptr) {return;}
	if (GetMoviePlayer()->IsMovieCurrentlyPlaying()) {return;}
	//
	if (LoadScreenMovieWidget.IsValid()) {
		LoadScreenMovieWidget->SetVisibility(EVisibility::Hidden);
	}///
	//
	OnFinishedLoadScreen();
	OnFinishLoadScreen.Broadcast();
}

void AHUD_SaviorUI::BreakLoadScreenMovie() {
	if ((USavior2::GetSaveProgress()>=100.f)&&(USavior2::GetLoadProgress()>=100.f)) {
		GetMoviePlayer()->OnMoviePlaybackFinished().Remove(OnFinishMovie);
		RemoveMovieLoadScreenHUD();
	} else {GetMoviePlayer()->PlayMovie();}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Slots Menu Interface:

void AHUD_SaviorUI::ShowSlotPickerHUD_Implementation(const ESlotPickerMode PickerMode) {
	const auto &Settings = GetMutableDefault<USavior2Settings>();
	//
	if (const auto &PC = GetOwningPlayerController()) {
		SlotPickerUI = CreateWidget<USlotPickerWidget>(PC,SlotPickerWidget);
		SlotPickerUI->PickerMode = PickerMode;
		//
		SlotPickerUI->AddToViewport(990);
		SlotPickerUI->SetVisibility(ESlateVisibility::Visible);
		//
		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
		//
		PC->SetInputMode(InputMode);
		SlotPickerUI->SetUserFocus(PC);
	}///
}

void AHUD_SaviorUI::HideSlotPickerHUD_Implementation() {
	if (SlotPickerUI->IsValidLowLevelFast()) {SlotPickerUI->RemoveFromViewport();}
	//
	if (const auto &PC = GetOwningPlayerController()) {
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		//
		PC->SetInputMode(InputMode);
	}///
	//
	FSlateApplication::Get().SetUserFocusToGameViewport(0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////