// Copyright VanstySanic. All Rights Reserved.

#include "Items/Scalability/VSSettingItem_ResolutionScale.h"
#include "Engine/RendererSettings.h"
#include "GameFramework/GameUserSettings.h"
#include "Libraries/VSGameplayLibrary.h"
#include "Types/VSSettingSystemTags.h"

UVSSettingItem_ResolutionScale::UVSSettingItem_ResolutionScale(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ItemInfo.SpecifyTag = EVSSettingItem::Scalability_ResolutionScale;
	ItemInfo.DisplayName = NSLOCTEXT("VSSettingSystem", "SettingItem.Scalability.ResolutionScale", "Resolution Scale");

	DesiredValueRange = FVector2D(FMath::Max(Scalability::MinResolutionScale, 25.f), Scalability::MaxResolutionScale);
	DesiredValueStep = 1.f;

	ContentTextFormat = FText::FromString("{0}%");
	DesiredFractionNum = 0;
}

void UVSSettingItem_ResolutionScale::Load_Implementation()
{
	SetResolutionScale(Scalability::GetQualityLevels().ResolutionQuality);
}

void UVSSettingItem_ResolutionScale::Apply_Implementation()
{
	Scalability::FQualityLevels QualityLevels = Scalability::GetQualityLevels();
	QualityLevels.ResolutionQuality = ResolutionScale;
	Scalability::SetQualityLevels(QualityLevels);
}

void UVSSettingItem_ResolutionScale::Confirm_Implementation()
{
	LastConfirmedResolutionScale = GetResolutionScale(EVSSettingItemValueSource::Current);
}

void UVSSettingItem_ResolutionScale::Save_Implementation()
{
	Scalability::SaveState(GGameUserSettingsIni);
}

float UVSSettingItem_ResolutionScale::GetValue_Implementation(EVSSettingItemValueSource::Type ValueType) const
{
	return GetResolutionScale(ValueType);
}

void UVSSettingItem_ResolutionScale::SetValue_Implementation(float InValue)
{
	SetResolutionScale(InValue);
}

void UVSSettingItem_ResolutionScale::SetResolutionScale(float InResolutionScale)
{
	if (FMath::IsNearlyEqual(ResolutionScale, InResolutionScale)) return;

	float PreviousValue = ResolutionScale;
	ResolutionScale = InResolutionScale;

	/** Reroute the 0.f condition to the render settings' default. */
	if (FMath::IsNearlyZero(ResolutionScale))
	{
		if (const URendererSettings* RendererSettings = GetDefault<URendererSettings>())
		{
			ResolutionScale = RendererSettings->DefaultManualScreenPercentage;
		}
	}

	ResolutionScale = FMath::Clamp(ResolutionScale, DesiredValueRange.X, DesiredValueRange.Y);
	
	if (!FMath::IsNearlyEqual(PreviousValue, ResolutionScale))
	{
		NotifyValueUpdate();
	}
}

float UVSSettingItem_ResolutionScale::GetResolutionScale(const EVSSettingItemValueSource::Type ValueSource) const
{
	if (!GEngine) return EWindowMode::NumWindowModes;
	UGameUserSettings* GameUserSettings = GEngine->GetGameUserSettings();

	float TargetResolutionScale = 0.0f;
	
	switch (ValueSource)
	{
	case EVSSettingItemValueSource::Default:
		TargetResolutionScale = GameUserSettings->GetDefaultResolutionScale();
		break;
		
	case EVSSettingItemValueSource::Current:
		TargetResolutionScale = Scalability::GetQualityLevels().ResolutionQuality;
		break;

	case EVSSettingItemValueSource::Settings:
		TargetResolutionScale = ResolutionScale;
		break;

	case EVSSettingItemValueSource::LastConfirmed:
		TargetResolutionScale = LastConfirmedResolutionScale;
		break;

	default:
		TargetResolutionScale = 0.f;
		break;
	}

	/** Reroute the 0.f condition to the render settings' default. */
	if (FMath::IsNearlyZero(TargetResolutionScale))
	{
		if (const URendererSettings* RendererSettings = GetDefault<URendererSettings>())
		{
			TargetResolutionScale = RendererSettings->DefaultManualScreenPercentage;
		}
	}

	return TargetResolutionScale;
}
