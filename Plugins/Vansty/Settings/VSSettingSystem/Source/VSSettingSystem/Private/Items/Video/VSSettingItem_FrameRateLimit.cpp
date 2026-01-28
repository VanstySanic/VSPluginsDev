// Copyright VanstySanic. All Rights Reserved.

#include "Items/Video/VSSettingItem_FrameRateLimit.h"

#include "VSSettingSystemConfig.h"
#include "GameFramework/GameUserSettings.h"
#include "Items/VSSettingSystemTags.h"

UVSSettingItem_FrameRateLimit::UVSSettingItem_FrameRateLimit(const FObjectInitializer& FObjectInitializer)
	: Super(FObjectInitializer)
{
	ItemTag = EVSSettingItem::Video::FrameRateLimit;
	ItemInfo.DisplayName = NSLOCTEXT("VS.SettingSystem.Item.Video.FrameRateLimit", "DisplayName", "Frame-Rate Limit");
	ConfigParams.ConfigSection = FString("/Script/Engine.GameUserSettings");
	ConfigParams.ConfigKeyName = FString("t.MaxFPS");

	DisplayNumericTextFormat = FText::FromString(TEXT("{0} FPS"));
	DisplayNumericFractionDigitRange = FIntPoint(0, 0);
	
	ExternalChangeActions.Empty();

	SetConsoleVariableName("t.MaxFPS");
}

bool UVSSettingItem_FrameRateLimit::IsValueValid_Implementation() const
{
	const float FrameRateLimit = GetFrameRateLimit(EVSSettingItemValueSource::System);
	return FrameRateLimit >= 0.f;
}

void UVSSettingItem_FrameRateLimit::Validate_Implementation()
{
	Super::Validate_Implementation();

	if (IsValueValid()) return;
	const float PrevFrameRateLimit = GetFrameRateLimit(EVSSettingItemValueSource::System);
	SetFloatValue(FMath::Max(PrevFrameRateLimit, 0.f));
}

FText UVSSettingItem_FrameRateLimit::ValueStringToText_Implementation(const FString& String) const
{
	const float Value = FCString::Atof(*String);
	if (FMath::IsNearlyZero(Value))
	{
		return UVSSettingSystemConfig::Get()->NoLimitsDisplayName;
	}
	
	return Super::ValueStringToText_Implementation(String);
}

void UVSSettingItem_FrameRateLimit::OnValueUpdated_Implementation()
{
	Super::OnValueUpdated_Implementation();

	/** Sync to GameUserSettings. */
	if (GEngine && GEngine->GameUserSettings)
	{
		const float FrameRateLimit = GetFrameRateLimit(EVSSettingItemValueSource::System);
		GEngine->GameUserSettings->SetFrameRateLimit(FrameRateLimit);
	}
}

void UVSSettingItem_FrameRateLimit::SetFrameRateLimit(float InLimit)
{
	SetFloatValue(InLimit);
}

float UVSSettingItem_FrameRateLimit::GetFrameRateLimit(EVSSettingItemValueSource::Type ValueSource) const
{
	return GetFloatValue(ValueSource);
}
