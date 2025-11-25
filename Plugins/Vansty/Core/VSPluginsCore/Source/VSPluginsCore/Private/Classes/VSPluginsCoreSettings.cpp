// Copyright VanstySanic. All Rights Reserved.

#include "Classes/VSPluginsCoreSettings.h"

UVSPluginsCoreSettings::UVSPluginsCoreSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SectionName = FName("VS Plugins Core Settings");
}

const UVSPluginsCoreSettings* UVSPluginsCoreSettings::Get()
{
	return GetDefault<UVSPluginsCoreSettings>();
}

const UVSPluginsCoreSettings* UVSPluginsCoreSettings::GetPluginsCoreSettings_VS()
{
	return Get();
}

FName UVSPluginsCoreSettings::GetCategoryName() const
{
	return FName("Game");
}