// Copyright VanstySanic. All Rights Reserved.

#include "Items/Video/VSSettingItem_WindowMode.h"
#include "VSPrivablic.h"
#include "Engine/GameEngine.h"
#include "GameFramework/GameUserSettings.h"

VS_DECLARE_PRIVABLIC_MEMBER(UGameUserSettings, LastConfirmedFullscreenMode, int32);

UVSSettingItem_WindowMode::UVSSettingItem_WindowMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSSettingItem_WindowMode::SetToByValueType_Implementation(const EVSSettingItemValueType::Type ValueType)
{
	SetWindowMode(GetWindowMode(ValueType));
}

void UVSSettingItem_WindowMode::Apply_Implementation()
{
#if !UE_SERVER
	if (FPlatformProperties::HasFixedResolution()) return;
	EWindowMode::Type WindowModeToApply = GetWindowMode(EVSSettingItemValueType::Settings);
	UGameEngine::ConditionallyOverrideSettings(GSystemResolution.ResX, GSystemResolution.ResY, WindowModeToApply);
	FSystemResolution::RequestResolutionChange(GSystemResolution.ResX, GSystemResolution.ResY, WindowModeToApply);
#endif
}

void UVSSettingItem_WindowMode::Confirm_Implementation()
{
	if (!GEngine) return;
	UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();
	VS_PRIVABLIC_MEMBER(GameUserSettings, UGameUserSettings, LastConfirmedFullscreenMode) = GetWindowMode(EVSSettingItemValueType::Current);
}

void UVSSettingItem_WindowMode::Save_Implementation()
{
	GConfig->SetInt(TEXT("Video"), TEXT("WindowMode"), GetWindowMode(EVSSettingItemValueType::Settings), GGameUserSettingsIni);
	GConfig->Flush(false, GGameUserSettingsIni);
}

bool UVSSettingItem_WindowMode::EqualsToByValueType_Implementation(const EVSSettingItemValueType::Type ValueType) const
{
	return GetWindowMode(EVSSettingItemValueType::Settings) != GetWindowMode(ValueType);
}

void UVSSettingItem_WindowMode::SetWindowMode(EWindowMode::Type InWindowMode)
{
	if (InWindowMode == GetWindowMode(EVSSettingItemValueType::Settings)) return;
	if (!GEngine) return;
	UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();
	GameUserSettings->SetFullscreenMode(InWindowMode);
}

EWindowMode::Type UVSSettingItem_WindowMode::GetWindowMode(EVSSettingItemValueType::Type ValueType) const
{
	if (!GEngine) return EWindowMode::NumWindowModes;
	UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();
	
	switch (ValueType)
	{
	case EVSSettingItemValueType::Default:
		return GameUserSettings->GetDefaultWindowMode();
			
	case EVSSettingItemValueType::Current:
		return GSystemResolution.WindowMode;
			
	case EVSSettingItemValueType::Settings:
		return GameUserSettings->GetFullscreenMode();

	case EVSSettingItemValueType::LastConfirmed:
		return GameUserSettings->GetLastConfirmedFullscreenMode();
		
	default:
		return EWindowMode::NumWindowModes;
	}
}
