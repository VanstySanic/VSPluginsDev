// Copyright VanstySanic. All Rights Reserved.

#include "WidgetBinders/Presets/VSCommonSettingItemOptionBasedWidgetBinder_Presets.h"
#include "GameplayTagContainer.h"
#include "Items/VSCommonSettingItem.h"
#include "Items/VSSettingSystemTags.h"
#include "Kismet/KismetSystemLibrary.h"

UVSCommonSettingItemOptionBasedWidgetBinder_Presets::UVSCommonSettingItemOptionBasedWidgetBinder_Presets(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

TArray<FString> UVSCommonSettingItemOptionBasedWidgetBinder_Presets::GenerateOptions_Implementation() const
{
	if (!GetCommonSettingItem()) return {};
	const FGameplayTag& ItemTag = GetCommonSettingItem()->GetItemTag();

	static FGameplayTag ScalabilityQualityLevelParentTag = UGameplayTagsManager::Get().RequestGameplayTagDirectParent(EVSSettingItem::Scalability::QualityLevel::AntiAliasing);

	if (ItemTag == EVSSettingItem::Video::WindowMode)
	{
		static TArray<FString> WindowModeOptions = TArray<FString>({ "0", "1", "2" });
		return WindowModeOptions;
	}
	if (ItemTag == EVSSettingItem::Video::ScreenResolution)
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
	if (ItemTag == EVSSettingItem::Video::VSync)
	{
		static TArray<FString> BooleanOptions = TArray<FString>({ "true", "false" });
		return BooleanOptions;
	}
	if (ItemTag.MatchesTag(ScalabilityQualityLevelParentTag))
	{
		static TArray<FString> ScalabilityQualityLevelOptions = TArray<FString>({ "0", "1", "2", "3", "4"});
		return ScalabilityQualityLevelOptions;
	}
	if (ItemTag == EVSSettingItem::Graphics::AntiAliasingMethod)
	{
		static TArray<FString> AntiAliasingMethodOptions = TArray<FString>({ "0", "1", "2", "3", "4"});
		return AntiAliasingMethodOptions;
	}
	if (ItemTag == EVSSettingItem::Video::FrameRateLimit)
	{
		FDisplayMetrics Metrics;
		FSlateApplication::Get().GetDisplayMetrics(Metrics);
		for (const FMonitorInfo& MonitorInfo : Metrics.MonitorInfo)
		{
			
		}
	}
	
	return Super::GenerateOptions_Implementation();
}

TArray<FString> UVSCommonSettingItemOptionBasedWidgetBinder_Presets::GenerateDisabledOptions_Implementation() const
{
	if (!GetCommonSettingItem()) return {};
	const FGameplayTag& ItemTag = GetCommonSettingItem()->GetItemTag();

	if (ItemTag == EVSSettingItem::Video::WindowMode)
	{
		static TArray<FString> WindowModeOptions = TArray<FString>({ "0", });
		return WindowModeOptions;
	}
	
	return Super::GenerateDisabledOptions_Implementation();
}
