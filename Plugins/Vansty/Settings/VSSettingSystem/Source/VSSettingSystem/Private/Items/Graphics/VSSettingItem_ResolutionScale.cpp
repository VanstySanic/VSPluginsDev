// Copyright VanstySanic. All Rights Reserved.

#include "Items/Graphics/VSSettingItem_ResolutionScale.h"
#include "Engine/RendererSettings.h"
#include "Items/VSSettingSystemTags.h"
#include "Types/Math/VSMath.h"

UVSSettingItem_ResolutionScale::UVSSettingItem_ResolutionScale(const FObjectInitializer& FObjectInitializer)
	: Super(FObjectInitializer)
{
	ItemTag = EVSSettingItem::Scalability::ResolutionScale;
	ItemInfo.DisplayName = NSLOCTEXT("VS.SettingSystem.Item.Scalability.ResolutionScale", "DisplayName", "Resolution Scale");
	ConfigParams.ConfigFileName = GIsEditor ? "Editor" : "GameUserSettings";
	ConfigParams.ConfigSection = FString("ScalabilityGroups");
	ConfigParams.ConfigKeyName = FString("sg.ResolutionQuality");

	SetConsoleVariableName(TEXT("sg.ResolutionQuality"));
}

bool UVSSettingItem_ResolutionScale::IsValueValid_Implementation() const
{
	const float ResolutionScale = GetResolutionScale(EVSSettingItemValueSource::System);
	return FVSMath::IsInRange(ResolutionScale, Scalability::MinResolutionScale, Scalability::MaxResolutionScale);
}

void UVSSettingItem_ResolutionScale::Validate_Implementation()
{
	Super::Validate_Implementation();

	if (IsValueValid()) return;
	const float PrevResolutionScale = GetResolutionScale(EVSSettingItemValueSource::System);
	SetResolutionScale(FMath::Clamp(PrevResolutionScale, Scalability::MinResolutionScale, Scalability::MaxResolutionScale));
}

float UVSSettingItem_ResolutionScale::GetFloatValue_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	if (!GEngine) return Super::GetFloatValue_Implementation(ValueSource);

	if (ValueType == EVSCommonSettingValueType::Float)
	{
		switch (ValueSource)
		{
		case EVSSettingItemValueSource::Default:
			if (const URendererSettings* RendererSettings = GetDefault<URendererSettings>())
			{
				return RendererSettings->DefaultManualScreenPercentage;
			}

		default: ;
		}
	}
		
	return Super::GetFloatValue_Implementation(ValueSource);
}

void UVSSettingItem_ResolutionScale::SetResolutionScale(float InResolutionScale)
{
	SetFloatValue(InResolutionScale);
}

float UVSSettingItem_ResolutionScale::GetResolutionScale(const EVSSettingItemValueSource::Type ValueSource) const
{
	return GetFloatValue(ValueSource);
}
