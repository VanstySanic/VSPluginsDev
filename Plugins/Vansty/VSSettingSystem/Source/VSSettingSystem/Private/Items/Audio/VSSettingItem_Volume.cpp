// Copyright VanstySanic. All Rights Reserved.

#include "Items/Audio/VSSettingItem_Volume.h"

#include "Kismet/GameplayStatics.h"

UVSSettingItem_Volume::UVSSettingItem_Volume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSSettingItem_Volume::Load_Implementation()
{
	Super::Load_Implementation();
}

void UVSSettingItem_Volume::Apply_Implementation()
{
	
}

void UVSSettingItem_Volume::Confirm_Implementation()
{
	Super::Confirm_Implementation();
}

void UVSSettingItem_Volume::Save_Implementation()
{
	Super::Save_Implementation();
}

void UVSSettingItem_Volume::SetValue_Implementation(float InValue)
{
	SetVolume(InValue);
}

float UVSSettingItem_Volume::GetValue_Implementation(EVSSettingItemValueSource::Type ValueSource) const
{
	return GetVolume(ValueSource);
}

void UVSSettingItem_Volume::SetVolume(float InVolume)
{

}

float UVSSettingItem_Volume::GetVolume(EVSSettingItemValueSource::Type ValueSource) const
{
	switch (ValueSource)
	{
	case EVSSettingItemValueSource::Default:
		return 0.f;
			
	case EVSSettingItemValueSource::Current:
		return 0.f;
			
	case EVSSettingItemValueSource::Settings:
		return 0.f;

	case EVSSettingItemValueSource::LastConfirmed:
		return 0.f;
		
	default:
		return 0.f;
	}
}

void UVSSettingItem_Volume::SetForceMute(bool bInForceMute)
{
}

bool UVSSettingItem_Volume::GetForceMute(EVSSettingItemValueSource::Type ValueSource) const
{
	switch (ValueSource)
	{
	case EVSSettingItemValueSource::Default:
		return false;
			
	case EVSSettingItemValueSource::Current:
		return false;
			
	case EVSSettingItemValueSource::Settings:
		return false;

	case EVSSettingItemValueSource::LastConfirmed:
		return false;
		
	default:
		return false;
	}
}
