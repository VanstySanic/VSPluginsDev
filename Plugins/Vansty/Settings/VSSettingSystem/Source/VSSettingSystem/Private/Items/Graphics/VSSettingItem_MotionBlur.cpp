// Copyright VanstySanic. All Rights Reserved.

#include "Items/Graphics/VSSettingItem_MotionBlur.h"
#include "VSSettingSystemConfig.h"
#include "Engine/RendererSettings.h"
#include "Items/VSSettingSystemTags.h"
#include "Types/Math/VSMath.h"

UVSSettingItem_MotionBlur::UVSSettingItem_MotionBlur(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ItemTag = EVSSettingItem::Graphics::MotionBlur;
	ItemInfo.DisplayName = NSLOCTEXT("VS.SettingSystem.Item.Graphics.MotionBlur", "DisplayName", "Motion Blur");
	ConfigSettings.FileName = GIsEditor ? "Editor" : "GameUserSettings";
	ConfigSettings.Section = "VS.Settings.Item.Graphics";
	ConfigSettings.PrimaryKey = "MotionBlurQuality";

	SetConsoleVariableName(TEXT("r.MotionBlurQuality"));
}

bool UVSSettingItem_MotionBlur::IsValueValid_Implementation() const
{
	const int32 CurrentQualityLevel = GetIntegerValue(EVSSettingItemValueSource::System);
	return FVSMath::IsInRange(CurrentQualityLevel, 0, 4);
}

void UVSSettingItem_MotionBlur::Validate_Implementation()
{
	Super::Validate_Implementation();

	if (IsValueValid()) return;
	const int32 CurrentQualityLevel = GetIntegerValue(EVSSettingItemValueSource::System);
	SetIntegerValue(FMath::Clamp(CurrentQualityLevel, 0, 4));
}

int32 UVSSettingItem_MotionBlur::GetIntegerValue_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	const int32 SuperValue = Super::GetIntegerValue_Implementation(ValueSource);
	if (!GEngine) return SuperValue;

	if (ValueType == EVSCommonSettingValueType::Integer)
	{
		switch (ValueSource)
		{
		case EVSSettingItemValueSource::Default:
			if (const URendererSettings* RendererSettings = GetDefault<URendererSettings>())
			{
				return RendererSettings->bDefaultFeatureMotionBlur ? SuperValue : 0;
			}

		default: ;
		}
	}

	return SuperValue;
}

FText UVSSettingItem_MotionBlur::ValueStringToText_Implementation(const FString& String) const
{
	const int32 Level = FCString::Atoi(*String);
	if (UVSSettingSystemConfig::Get()->MotionBlueQualityLevelNames.Contains(Level))
	{
		return UVSSettingSystemConfig::Get()->MotionBlueQualityLevelNames.FindRef(Level);
	}
	
	return Super::ValueStringToText_Implementation(String);
}

void UVSSettingItem_MotionBlur::SetMotionBlurQualityLevel(int32 QualityLevel)
{
	SetIntegerValue(QualityLevel);
}

int32 UVSSettingItem_MotionBlur::GetMotionBlurQualityLevel(EVSSettingItemValueSource::Type ValueSource) const
{
	return GetIntegerValue(ValueSource);
}

bool UVSSettingItem_MotionBlur::GetMotionBlurEnabled(EVSSettingItemValueSource::Type ValueSource) const
{
	return GetBooleanValue(ValueSource);
}
