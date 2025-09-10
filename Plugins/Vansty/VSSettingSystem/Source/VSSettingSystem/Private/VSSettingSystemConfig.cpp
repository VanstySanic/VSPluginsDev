// Copyright VanstySanic. All Rights Reserved.

#include "VSSettingSystemConfig.h"

UVSSettingSystemConfig::UVSSettingSystemConfig(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	EnableDisableTexts = TMap<bool, FText>
	{
		{ true, NSLOCTEXT("VSSettingSystem", "Settings.Text.Enabled", "Enabled") },
		{ false, NSLOCTEXT("VSSettingSystem", "Settings.Text.Enabled", "Disabled") }
	};

	OnOffTexts = TMap<bool, FText>
	{
		{ true, NSLOCTEXT("VSSettingSystem", "Settings.Text.On", "On") },
		{ false, NSLOCTEXT("VSSettingSystem", "Settings.Text.Off", "Off") }
	};

	NoLimitsText = NSLOCTEXT("VSSettingSystem", "Settings.Text.NoLimits", "No Limits");
	
	// PercentageTextFormat = NSLOCTEXT("VSSettingSystem", "Settings.Text.Percentage", "%d%");
}

#if WITH_EDITOR
FText UVSSettingSystemConfig::GetSectionText() const
{
	return FText::FromString("VSSettingSystem");
}
#endif