// Copyright VanstySanic. All Rights Reserved.

#include "Items/Scalability/VSSettingItem_ResolutionScale.h"
#include "GameFramework/GameUserSettings.h"
#include "Types/VSSettingSystemTags.h"

UVSSettingItem_ResolutionScale::UVSSettingItem_ResolutionScale(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ItemInfo.SpecifyTag = EVSSettingItem::Scalability_ResolutionScale;
	ItemInfo.DisplayName = NSLOCTEXT("VSSettingSystem", "SettingItem.Scalability.ResolutionScale", "Resolution Scale");
}

void UVSSettingItem_ResolutionScale::Load_Implementation()
{
	ResolutionScale = Scalability::GetQualityLevels().ResolutionQuality;
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
	LastConfirmedResolutionScale = GetResolutionScale(EVSSettingItemValueSource::Current);
}

void UVSSettingItem_ResolutionScale::Save_Implementation()
{
	Scalability::SaveState(GGameUserSettingsIni);
}

void UVSSettingItem_ResolutionScale::SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource)
{
	SetResolutionScale(GetResolutionScale(ValueSource));
}

bool UVSSettingItem_ResolutionScale::EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	return ResolutionScale != GetResolutionScale(ValueSource);
}

void UVSSettingItem_ResolutionScale::SetResolutionScale(float InResolutionScale)
{
	if (FMath::IsNearlyEqual(ResolutionScale, InResolutionScale)) return;
	if (ResolutionScale < Scalability::MinResolutionScale || ResolutionScale > Scalability::MaxResolutionScale) return;
	ResolutionScale = InResolutionScale;
	NotifyUpdate();
}

float UVSSettingItem_ResolutionScale::GetResolutionScale(const EVSSettingItemValueSource::Type ValueSource) const
{
	if (!GEngine) return EWindowMode::NumWindowModes;
	UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();

	switch (ValueSource)
	{
	case EVSSettingItemValueSource::Default:
		return GameUserSettings->GetDefaultResolutionScale();
			
	case EVSSettingItemValueSource::Current:
		return Scalability::GetQualityLevels().ResolutionQuality;
			
	case EVSSettingItemValueSource::Settings:
		return ResolutionScale;

	case EVSSettingItemValueSource::LastConfirmed:
		return LastConfirmedResolutionScale;
		
	default:
		return 0.f;
	}
}
