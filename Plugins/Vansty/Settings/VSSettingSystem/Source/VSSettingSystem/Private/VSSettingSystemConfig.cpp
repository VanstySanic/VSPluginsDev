// Copyright VanstySanic. All Rights Reserved.

#include "VSSettingSystemConfig.h"

UVSSettingSystemConfig::UVSSettingSystemConfig(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UVSSettingSystemConfig* UVSSettingSystemConfig::Get()
{
	return GetDefault<UVSSettingSystemConfig>();
}

const UVSSettingSystemConfig* UVSSettingSystemConfig::GetSettingSystemConfig_VS()
{
	return GetDefault<UVSSettingSystemConfig>();
}

FName UVSSettingSystemConfig::GetCategoryName() const
{
	return FName("Engine");
}
