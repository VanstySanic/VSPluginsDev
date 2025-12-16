// Copyright VanstySanic. All Rights Reserved.

#include "Items/Video/VSSettingItem_VSync.h"
#include "GameFramework/GameUserSettings.h"
#include "Items/VSSettingSystemTags.h"

UVSSettingItem_VSync::UVSSettingItem_VSync(const FObjectInitializer& FObjectInitializer)
	: Super(FObjectInitializer)
{
	ItemTag = EVSSettingItem::Video::VSync;
	ItemInfo.DisplayName = NSLOCTEXT("VS.SettingSystem.Item.Video.VSync", "DisplayName", "VSync");
	ConfigParams.ConfigSection = FString("/Script/Engine.GameUserSettings");
	ConfigParams.ConfigKeyName = FString("bUseVSync");

	SetConsoleVariableName("r.VSync");
}

void UVSSettingItem_VSync::OnValueUpdated_Implementation()
{
	Super::OnValueUpdated_Implementation();

	/** Sync to GameUserSettings. */
	if (GEngine && GEngine->GameUserSettings)
	{
		const bool bVSyncEnabled = GetVSyncEnabled(EVSSettingItemValueSource::System);
		GEngine->GameUserSettings->SetVSyncEnabled(bVSyncEnabled);
	}
}

void UVSSettingItem_VSync::SetVSyncEnabled(bool bEnabled)
{
	SetBooleanValue(bEnabled);
}

bool UVSSettingItem_VSync::GetVSyncEnabled(EVSSettingItemValueSource::Type ValueSource) const
{
	return GetBooleanValue(ValueSource);
}
