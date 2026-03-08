// Copyright VanstySanic. All Rights Reserved.

#include "VSMoverSystemSettings.h"

UVSMoverSystemSettings::UVSMoverSystemSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

#if WITH_EDITOR
FText UVSMoverSystemSettings::GetSectionText() const
{
	return FText::FromString("VSMoverSystemSettings");
}
#endif

const UVSMoverSystemSettings* UVSMoverSystemSettings::Get()
{
	return GetDefault<UVSMoverSystemSettings>();
}

const UVSMoverSystemSettings* UVSMoverSystemSettings::GetMoverSystemSettingsVS()
{
	return GetDefault<UVSMoverSystemSettings>();
}