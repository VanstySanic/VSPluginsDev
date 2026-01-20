// Copyright VanstySanic. All Rights Reserved.

#include "VSSettingSystemConfig.h"
#include "VSSettingSubsystem.h"

#define LOCTEXT_NAMESPACE "VSSettingSystemConfig"

UVSSettingSystemConfig::UVSSettingSystemConfig(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NoLimitsDisplayName = LOCTEXT("DisplayName.State.NoLimits", "No Limits");
	
	OnOffDisplayNames = TMap<bool, FText>
	{
		{ true, LOCTEXT("DisplayName.State.On", "On") },
		{ false, LOCTEXT("DisplayName.State.False", "Off") },
	};

	EnabledStateDisplayNames = TMap<bool, FText>
	{
		{ true, LOCTEXT("DisplayName.State.Enabled", "Enabled") },
		{ false, LOCTEXT("DisplayName.State.Disabled", "Disabled") },
	};

	WindowModeDisplayNames = TMap<TEnumAsByte<EWindowMode::Type>, FText>
	{
		{ EWindowMode::Fullscreen, LOCTEXT("DisplayName.State.WindowMode.Fullscreen", "Fullscreen") },
		{ EWindowMode::WindowedFullscreen, LOCTEXT("DisplayName.State.WindowMode.WindowedFullscreen", "Windowed-Fullscreen") },
		{ EWindowMode::Windowed, LOCTEXT("DisplayName.State.WindowMode.Windowed", "Windowed") },
	};

	AntiAliasingMethodDisplayNames = TMap<TEnumAsByte<EAntiAliasingMethod>, FText>
	{
		{ AAM_None, LOCTEXT("DisplayName.State.AntiAliasingMethod.None", "None") },
		{ AAM_FXAA, LOCTEXT("DisplayName.State.AntiAliasingMethod.FXAA", "FXAA") },
		{ AAM_TemporalAA, LOCTEXT("DisplayName.State.AntiAliasingMethod.TemporalAA", "TAA") },
		{ AAM_MSAA, LOCTEXT("DisplayName.State.AntiAliasingMethod.Fullscreen", "MSAA") },
		{ AAM_TSR, LOCTEXT("DisplayName.State.AntiAliasingMethod.Fullscreen", "TSR") },
	};
}

const UVSSettingSystemConfig* UVSSettingSystemConfig::Get()
{
	return GetDefault<UVSSettingSystemConfig>();
}

const UVSSettingSystemConfig* UVSSettingSystemConfig::GetSettingSystemConfig_VS()
{
	return GetDefault<UVSSettingSystemConfig>();
}

void UVSSettingSystemConfig::PostLoad()
{
	Super::PostLoad();

#if WITH_EDITORONLY_DATA
	EditorSettingItemAgentClasses = SettingItemAgentClasses;
#endif
}

#if WITH_EDITOR
void UVSSettingSystemConfig::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UVSSettingSystemConfig, SettingItemAgentClasses))
	{
		if (SettingItemAgentClasses != EditorSettingItemAgentClasses)
		{
			if (UVSSettingSubsystem* SettingSubsystem = UVSSettingSubsystem::Get())
			{
				SettingSubsystem->RefreshEditorDirectSettingItemAgents();
			}

			EditorSettingItemAgentClasses = SettingItemAgentClasses;
		}
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

FName UVSSettingSystemConfig::GetCategoryName() const
{
	return FName("Engine");
}
