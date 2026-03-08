// Copyright VanstySanic. All Rights Reserved.

#include "Classes/Settings/VSPluginsCoreGameSettings.h"

UVSPluginsCoreGameSettings::UVSPluginsCoreGameSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SectionName = FName("VS Plugins Core Game Settings");
	
}

const UVSPluginsCoreGameSettings* UVSPluginsCoreGameSettings::Get()
{
	return GetDefault<UVSPluginsCoreGameSettings>();
}

const UVSPluginsCoreGameSettings* UVSPluginsCoreGameSettings::GetPluginsCoreGameSettingsVS()
{
	return Get();
}

FName UVSPluginsCoreGameSettings::GetCategoryName() const
{
	return FName("Game");
}