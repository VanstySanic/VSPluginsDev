// Copyright VanstySanic. All Rights Reserved.

#include "Items/Video/VSSettingItem_ScreenResolution.h"

UVSSettingItem_ScreenResolution::UVSSettingItem_ScreenResolution(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSSettingItem_ScreenResolution::Apply_Implementation()
{
	Super::Apply_Implementation();
}

void UVSSettingItem_ScreenResolution::Confirm_Implementation()
{
	Super::Confirm_Implementation();
}

void UVSSettingItem_ScreenResolution::Save_Implementation()
{
	Super::Save_Implementation();
}

void UVSSettingItem_ScreenResolution::SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource)
{
	Super::SetToBySource_Implementation(ValueSource);
}

bool UVSSettingItem_ScreenResolution::EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	return Super::EqualsToBySource_Implementation(ValueSource);
}

void UVSSettingItem_ScreenResolution::SetScreenResolution(const FIntPoint& Resolution)
{
}

FIntPoint UVSSettingItem_ScreenResolution::GetScreenResolution(EVSSettingItemValueSource::Type ValueSource) const
{
	return FIntPoint::ZeroValue;
}
