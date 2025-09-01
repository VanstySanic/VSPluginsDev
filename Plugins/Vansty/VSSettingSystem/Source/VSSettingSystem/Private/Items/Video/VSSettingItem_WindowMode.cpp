// Copyright VanstySanic. All Rights Reserved.

#include "Items/Video/VSSettingItem_WindowMode.h"
#include "VSPrivablic.h"
#include "Engine/GameEngine.h"
#include "GameFramework/GameUserSettings.h"
#include "Types/VSSettingSystemTags.h"

VS_DECLARE_PRIVABLIC_MEMBER(UGameUserSettings, LastConfirmedFullscreenMode, int32);

UVSSettingItem_WindowMode::UVSSettingItem_WindowMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ItemInfo.SpecifyTag = EVSSettingItem::Video_WindowMode;
	ItemInfo.DisplayName = NSLOCTEXT("VSSettingSystem", "SettingItem.WindowMode", "Window Mode");

	WindowModeNames.Emplace(EWindowMode::WindowedFullscreen, NSLOCTEXT("VSSettingSystem", "SettingItem.WindowMode.WindowedFullscreen", "Windowed-Fullscreen"));
	WindowModeNames.Emplace(EWindowMode::Fullscreen, NSLOCTEXT("VSSettingSystem", "SettingItem.WindowMode.Fullscreen", "Fullscreen"));
	WindowModeNames.Emplace(EWindowMode::Windowed, NSLOCTEXT("VSSettingSystem", "SettingItem.WindowMode.Windowed", "Windowed"));
}

void UVSSettingItem_WindowMode::Apply_Implementation()
{
#if !UE_SERVER
	if (FPlatformProperties::HasFixedResolution()) return;
	EWindowMode::Type WindowModeToApply = GetWindowMode(EVSSettingItemValueSource::Settings);
	UGameEngine::ConditionallyOverrideSettings(GSystemResolution.ResX, GSystemResolution.ResY, WindowModeToApply);
	FSystemResolution::RequestResolutionChange(GSystemResolution.ResX, GSystemResolution.ResY, WindowModeToApply);
#endif
}

void UVSSettingItem_WindowMode::Confirm_Implementation()
{
	if (!GEngine) return;
	UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();
	VS_PRIVABLIC_MEMBER(GameUserSettings, UGameUserSettings, LastConfirmedFullscreenMode) = GetWindowMode(EVSSettingItemValueSource::Current);
}

void UVSSettingItem_WindowMode::Save_Implementation()
{
	GConfig->SetInt(TEXT("/Script/Engine.GameUserSettings"), TEXT("FullscreenMode"), GetWindowMode(EVSSettingItemValueSource::Settings), GGameUserSettingsIni);
	GConfig->Flush(false, GGameUserSettingsIni);
}

void UVSSettingItem_WindowMode::SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource)
{
	SetWindowMode(GetWindowMode(ValueSource));
}

bool UVSSettingItem_WindowMode::EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	return GetWindowMode(EVSSettingItemValueSource::Settings) != GetWindowMode(ValueSource);
}

void UVSSettingItem_WindowMode::SetWindowMode(EWindowMode::Type InWindowMode)
{
	if (InWindowMode == GetWindowMode(EVSSettingItemValueSource::Settings)) return;
	if (!GEngine) return;
	UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();
	GameUserSettings->SetFullscreenMode(InWindowMode);
	NotifyUpdate();
}

EWindowMode::Type UVSSettingItem_WindowMode::GetWindowMode(EVSSettingItemValueSource::Type ValueSource) const
{
	if (!GEngine) return EWindowMode::NumWindowModes;
	UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();
	
	switch (ValueSource)
	{
	case EVSSettingItemValueSource::Default:
		return GameUserSettings->GetDefaultWindowMode();
			
	case EVSSettingItemValueSource::Current:
		return GSystemResolution.WindowMode;
			
	case EVSSettingItemValueSource::Settings:
		return GameUserSettings->GetFullscreenMode();

	case EVSSettingItemValueSource::LastConfirmed:
		return GameUserSettings->GetLastConfirmedFullscreenMode();
		
	default:
		return EWindowMode::NumWindowModes;
	}
}
