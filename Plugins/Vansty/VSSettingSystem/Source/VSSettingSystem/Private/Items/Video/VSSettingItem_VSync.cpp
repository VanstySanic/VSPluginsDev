// Copyright VanstySanic. All Rights Reserved.

#include "Items/Video/VSSettingItem_VSync.h"
#include "VSSettingSystemConfig.h"
#include "GameFramework/GameUserSettings.h"
#include "Types/VSSettingSystemTags.h"

UVSSettingItem_VSync::UVSSettingItem_VSync(const FObjectInitializer& FObjectInitializer)
	: Super(FObjectInitializer)
{
	ItemInfo.SpecifyTag = EVSSettingItem::Video_VSync;
	ItemInfo.DisplayName = NSLOCTEXT("VSSettingSystem", "SettingItem.VSync", "VSync");

	NamedValues = UVSSettingSystemConfig::Get()->EnableDisableTexts;
}

void UVSSettingItem_VSync::Load_Implementation()
{
	bool LoadVSyncEnabled = false;
	if (GConfig->GetBool(TEXT("/Script/Engine.GameUserSettings"), TEXT("bUseVSync"), LoadVSyncEnabled, GGameUserSettingsIni))
	{
		SetVSyncEnabled(LoadVSyncEnabled);
	}
}

void UVSSettingItem_VSync::Apply_Implementation()
{
	static auto CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.VSync"));
	CVar->SetWithCurrentPriority(GetVSyncEnabled(EVSSettingItemValueSource::Settings));
}

void UVSSettingItem_VSync::Confirm_Implementation()
{
	LastConfirmedVSyncEnabled = GetVSyncEnabled(EVSSettingItemValueSource::Settings);
}

void UVSSettingItem_VSync::Save_Implementation()
{
	GConfig->SetBool(TEXT("/Script/Engine.GameUserSettings"), TEXT("bUseVSync"), GetVSyncEnabled(EVSSettingItemValueSource::Settings), GGameUserSettingsIni);
	GConfig->Flush(false, GGameUserSettingsIni);
}

void UVSSettingItem_VSync::SetValue_Implementation(bool bInValue)
{
	SetVSyncEnabled(bInValue);
}

bool UVSSettingItem_VSync::GetValue_Implementation(EVSSettingItemValueSource::Type ValueSource) const
{
	return GetVSyncEnabled(ValueSource);
}

void UVSSettingItem_VSync::SetVSyncEnabled(bool bEnabled)
{
	if (!GEngine) return;
	if (bEnabled == GetVSyncEnabled(EVSSettingItemValueSource::Settings)) return;
	UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();
	GameUserSettings->SetVSyncEnabled(bEnabled);
	NotifyValueUpdate();
}

bool UVSSettingItem_VSync::GetVSyncEnabled(EVSSettingItemValueSource::Type ValueSource) const
{
	if (!GEngine) return 0.f;
	UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();

	static auto CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.VSync"));
	
	switch (ValueSource)
	{
	case EVSSettingItemValueSource::Default:
		return false;
			
	case EVSSettingItemValueSource::Current:
		return CVar->GetBool();
			
	case EVSSettingItemValueSource::Settings:
		return GameUserSettings->bUseVSync;

	case EVSSettingItemValueSource::LastConfirmed:
		return LastConfirmedVSyncEnabled;
		
	default:
		return false;
	}
}
