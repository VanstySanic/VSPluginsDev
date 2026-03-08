// Copyright VanstySanic. All Rights Reserved.

#include "VSCharacterMovementSystemSettings.h"

UVSCharacterMovementSystemSettings::UVSCharacterMovementSystemSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

#if WITH_EDITOR
FText UVSCharacterMovementSystemSettings::GetSectionText() const
{
	return FText::FromString("VSCharacterMovementSystemSettings");
}
#endif

const UVSCharacterMovementSystemSettings* UVSCharacterMovementSystemSettings::Get()
{
	return GetDefault<UVSCharacterMovementSystemSettings>();
}

const UVSCharacterMovementSystemSettings* UVSCharacterMovementSystemSettings::GetCharacterMovementSystemSettingsVS()
{
	return GetDefault<UVSCharacterMovementSystemSettings>();
}