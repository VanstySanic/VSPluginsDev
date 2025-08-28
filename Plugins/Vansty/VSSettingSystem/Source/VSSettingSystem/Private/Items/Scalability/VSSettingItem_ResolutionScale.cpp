// Copyright VanstySanic. All Rights Reserved.

#include "Items/Scalability/VSSettingItem_ResolutionScale.h"
#include "GameFramework/GameUserSettings.h"

UVSSettingItem_ResolutionScale::UVSSettingItem_ResolutionScale(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UVSSettingItem_ResolutionScale::Load_Implementation()
{
	ResolutionScale = Scalability::GetQualityLevels().ResolutionQuality;
}

void UVSSettingItem_ResolutionScale::SetToByValueType_Implementation(const EVSSettingItemValueType::Type ValueType)
{
	SetResolutionScale(GetResolutionScale(ValueType));
}

void UVSSettingItem_ResolutionScale::Apply_Implementation()
{
	Scalability::FQualityLevels QualityLevels = Scalability::GetQualityLevels();
	QualityLevels.ResolutionQuality = ResolutionScale;
	Scalability::SetQualityLevels(QualityLevels);
}

void UVSSettingItem_ResolutionScale::Validate_Implementation()
{
	ResolutionScale = FMath::Clamp(ResolutionScale, Scalability::MinResolutionScale, Scalability::MaxResolutionScale);
}

void UVSSettingItem_ResolutionScale::Confirm_Implementation()
{
	LastConfirmedResolutionScale = GetResolutionScale(EVSSettingItemValueType::Current);
}

void UVSSettingItem_ResolutionScale::Save_Implementation()
{
	Scalability::SaveState(GGameUserSettingsIni);
}

bool UVSSettingItem_ResolutionScale::EqualsToByValueType_Implementation(const EVSSettingItemValueType::Type ValueType) const
{
	return ResolutionScale != GetResolutionScale(ValueType);
}

void UVSSettingItem_ResolutionScale::SetResolutionScale(float InResolutionScale)
{
	if (FMath::IsNearlyEqual(ResolutionScale, InResolutionScale)) return;
	if (ResolutionScale < Scalability::MinResolutionScale || ResolutionScale > Scalability::MaxResolutionScale) return;
	ResolutionScale = InResolutionScale;
}

float UVSSettingItem_ResolutionScale::GetResolutionScale(const EVSSettingItemValueType::Type ValueType) const
{
	if (!GEngine) return EWindowMode::NumWindowModes;
	UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();

	switch (ValueType)
	{
	case EVSSettingItemValueType::Default:
		return GameUserSettings->GetDefaultResolutionScale();
			
	case EVSSettingItemValueType::Current:
		return Scalability::GetQualityLevels().ResolutionQuality;
			
	case EVSSettingItemValueType::Settings:
		return ResolutionScale;

	case EVSSettingItemValueType::LastConfirmed:
		return LastConfirmedResolutionScale;
		
	default:
		return 0.f;
	}
}
