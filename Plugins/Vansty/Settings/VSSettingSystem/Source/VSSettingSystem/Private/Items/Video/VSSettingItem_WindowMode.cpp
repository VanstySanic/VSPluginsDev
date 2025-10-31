// Copyright VanstySanic. All Rights Reserved.

#include "Items/Video/VSSettingItem_WindowMode.h"
#include "VSPrivablic.h"
#include "Engine/GameEngine.h"
#include "GameFramework/GameUserSettings.h"
#include "Libraries/VSGameplayLibrary.h"
#include "Types/VSSettingSystemTags.h"

VS_DECLARE_PRIVABLIC_MEMBER(UGameUserSettings, LastConfirmedFullscreenMode, int32);

UVSSettingItem_WindowMode::UVSSettingItem_WindowMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ItemInfo.SpecifyTag = EVSSettingItem::Video_WindowMode;
	ItemInfo.DisplayName = NSLOCTEXT("VSSettingSystem", "SettingItem.WindowMode", "Window Mode");

	DesiredValueRange = FIntPoint(0, 2);

	NamedValues.Empty();
	NamedValues.Emplace(EWindowMode::Fullscreen, NSLOCTEXT("VSSettingSystem", "SettingItem.WindowMode.Fullscreen", "Fullscreen"));
	NamedValues.Emplace(EWindowMode::WindowedFullscreen, NSLOCTEXT("VSSettingSystem", "SettingItem.WindowMode.WindowedFullscreen", "Windowed-Fullscreen"));
	NamedValues.Emplace(EWindowMode::Windowed, NSLOCTEXT("VSSettingSystem", "SettingItem.WindowMode.Windowed", "Windowed"));
}

void UVSSettingItem_WindowMode::Load_Implementation()
{
	int32 LoadWindowMode = 0;
	if (GConfig->GetInt(TEXT("/Script/Engine.GameUserSettings"), TEXT("FullscreenMode"), LoadWindowMode, GGameUserSettingsIni))
	{
		SetWindowMode(EWindowMode::Type(LoadWindowMode));
	}
}

void UVSSettingItem_WindowMode::Validate_Implementation()
{
	if (GetWindowMode(EVSSettingItemValueSource::Settings) == EWindowMode::NumWindowModes)
	{
		if (!GEngine) return;
		UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();
		SetWindowMode(GameUserSettings->GetPreferredFullscreenMode());
	}
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
	GConfig->SetInt(TEXT("/Script/Engine.GameUserSettings"), TEXT("LastConfirmedFullscreenMode"), GetWindowMode(EVSSettingItemValueSource::LastConfirmed), GGameUserSettingsIni);
	GConfig->Flush(false, GGameUserSettingsIni);
}

void UVSSettingItem_WindowMode::SetValue_Implementation(int32 InValue)
{
	SetWindowMode(EWindowMode::Type(InValue));
}

int32 UVSSettingItem_WindowMode::GetValue_Implementation(EVSSettingItemValueSource::Type ValueType) const
{
	return GetWindowMode(ValueType);
}

void UVSSettingItem_WindowMode::SetWindowMode(EWindowMode::Type InWindowMode)
{
	if (InWindowMode == GetWindowMode(EVSSettingItemValueSource::Settings)) return;
	if (!GEngine) return;
	UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();
	GameUserSettings->SetFullscreenMode(InWindowMode);
	NotifyValueUpdate();
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