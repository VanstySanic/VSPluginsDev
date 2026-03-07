// Copyright VanstySanic. All Rights Reserved.

#include "Mediator/Binders/Presets/VSCommonSettingItemOptionBasedWidgetBinder_CommonPresets.h"
#include "AudioMixerBlueprintLibrary.h"
#include "GameplayTagContainer.h"
#include "Classes/Libraries/VSPlatformLibrary.h"
#include "Items/VSCommonSettingItem.h"
#include "Items/VSSettingSystemTags.h"
#include "Kismet/KismetSystemLibrary.h"

UVSCommonSettingItemOptionBasedWidgetBinder_CommonPresets::UVSCommonSettingItemOptionBasedWidgetBinder_CommonPresets(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

TArray<FString> UVSCommonSettingItemOptionBasedWidgetBinder_CommonPresets::GenerateOptions_Implementation() const
{
	if (!GetSettingItem_Native()) return {};
	const FGameplayTag ItemIdentifier = GetSettingItem_Native()->GetItemIdentifier();

	static FGameplayTag ScalabilityQualityLevelParentTag = UGameplayTagsManager::Get().RequestGameplayTagDirectParent(EVSSettingItem::Scalability::QualityLevel::AntiAliasing);

	if (ItemIdentifier == EVSSettingItem::Video::WindowMode)
	{
		static TArray<FString> WindowModeOptions = TArray<FString>({ "0", "1", "2" });
		return WindowModeOptions;
	}
	if (ItemIdentifier == EVSSettingItem::Video::ScreenResolution)
	{
		TArray<FString> ResolutionOptions;
		TArray<FIntPoint> SupportedResolutions;
		UKismetSystemLibrary::GetSupportedFullscreenResolutions(SupportedResolutions);

		for (const FIntPoint& SupportedResolution : SupportedResolutions)
		{
			ResolutionOptions.Add(SupportedResolution.ToString());
		}
		return ResolutionOptions;
	}
	if (ItemIdentifier == EVSSettingItem::Video::VSync)
	{
		static TArray<FString> ZeroOneOptions = TArray<FString>({ "0", "1" });
		return ZeroOneOptions;
	}
	if (ItemIdentifier.MatchesTag(ScalabilityQualityLevelParentTag)
		|| ItemIdentifier == EVSSettingItem::Graphics::AntiAliasingMethod
		|| ItemIdentifier == EVSSettingItem::Graphics::MotionBlur)
	{
		static TArray<FString> FiveLevelOptions = TArray<FString>({ "0", "1", "2", "3", "4"});
		return FiveLevelOptions;
	}
	if (ItemIdentifier == EVSSettingItem::Video::Monitor)
	{
		TArray<FString> MonitorIDs;
		FDisplayMetrics Metrics;
		FSlateApplication::Get().GetDisplayMetrics(Metrics);
		for (const FMonitorInfo& MonitorInfo : Metrics.MonitorInfo)
		{
			MonitorIDs.Add(MonitorInfo.ID);
		}
		return MonitorIDs;
	}
	if (ItemIdentifier == EVSSettingItem::Video::FrameRateLimit)
	{
		static TArray<float> FrameRateLimits = TArray<float>
		{
			30.f, 60.f, 75.f, 90.f, 120.f, 144.f, 165.f, 240.f, 360.f, 0.f
		};
		
		TArray<FString> OutOptions;
		for (const float FrameRateLimit : FrameRateLimits)
		{
			OutOptions.Add(FString::Printf(TEXT("%f"), FrameRateLimit));
		}
		
		return OutOptions;
	}
	if (ItemIdentifier == EVSSettingItem::Audio::Device::Output)
	{
		TArray<FString> DeviceOptions;
		const TArray<FAudioOutputDeviceInfo> DeviceInfos = UVSPlatformLibrary::GetAvailableAudioOutputDeviceInfos();
		for (const FAudioOutputDeviceInfo& DeviceInfo : DeviceInfos)
		{
			DeviceOptions.Add(DeviceInfo.DeviceId);
		}

		return DeviceOptions;
	}
	
	return Super::GenerateOptions_Implementation();
}

TArray<FString> UVSCommonSettingItemOptionBasedWidgetBinder_CommonPresets::GenerateDisabledOptions_Implementation() const
{
	if (!GetSettingItem_Native()) return {};
	const FGameplayTag ItemIdentifier = GetSettingItem_Native()->GetItemIdentifier();
	
	return Super::GenerateDisabledOptions_Implementation();
}
