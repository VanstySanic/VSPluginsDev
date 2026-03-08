// Copyright VanstySanic. All Rights Reserved.

#include "Mediator/Binders/Presets/VSSettingItemEnabledStateControlWidgetBinder_CommonPresets.h"
#include "VSSettingSubsystem.h"
#include "VSSettingSystemUtils.h"
#include "Items/VSCommonSettingItem.h"
#include "Items/VSSettingItemBase.h"
#include "Items/VSSettingSystemTags.h"
#include "Items/Video/VSSettingItem_Monitor.h"
#include "Items/Video/VSSettingItem_ScreenResolution.h"
#include "Items/Video/VSSettingItem_WindowMode.h"

UVSSettingItemEnabledStateControlWidgetBinder_CommonPresets::UVSSettingItemEnabledStateControlWidgetBinder_CommonPresets(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UVSSettingItemEnabledStateControlWidgetBinder_CommonPresets::OnAnySettingItemUpdated_Implementation(UVSSettingItemBase* SettingItem)
{
	Super::OnAnySettingItemUpdated_Implementation(SettingItem);

	if (AutoRefreshUpdatedItems.HasTag(SettingItem->GetItemIdentifier()))
	{
		RefreshEnabledState();
	}
}

bool UVSSettingItemEnabledStateControlWidgetBinder_CommonPresets::GetDesiredEnabledState_Implementation()
{
	UVSSettingSubsystem* SettingSubsystem = UVSSettingSubsystem::Get();
	if (!GetSettingItem_Native() || !SettingSubsystem) return Super::GetDesiredEnabledState_Implementation();
	const FGameplayTag ItemIdentifier = GetSettingItem_Native()->GetItemIdentifier();

	if (ItemIdentifier == VS::Settings::Item::Video::ScreenResolution)
	{
		if (UVSSettingItem_ScreenResolution* ScreenResolutionItem = Cast<UVSSettingItem_ScreenResolution>(GetSettingItem_Native()))
		{
			if (UVSSettingItem_WindowMode* WindowModeItem = Cast<UVSSettingItem_WindowMode>(UVSSettingSystemUtils::GetSettingItemByIdentifier(VS::Settings::Item::Video::WindowMode)))
			{
				if (WindowModeItem->GetWindowMode(EVSSettingItemValueSource::System) == EWindowMode::WindowedFullscreen && ScreenResolutionItem->DisableInWindowedFullscreenMode)
				{
					return false;
				}
			}
		}
	}
	else if (ItemIdentifier == VS::Settings::Item::Video::Monitor)
	{
		if (UVSSettingItem_Monitor* MonitorItem = Cast<UVSSettingItem_Monitor>(GetSettingItem_Native()))
		{
			if (UVSSettingItem_WindowMode* WindowModeItem = Cast<UVSSettingItem_WindowMode>(UVSSettingSystemUtils::GetSettingItemByIdentifier(VS::Settings::Item::Video::WindowMode)))
			{
				const EWindowMode::Type WindowMode = WindowModeItem->GetWindowMode();
				if (WindowMode == EWindowMode::Windowed || (!MonitorItem->bOverrideDesiredFullscreenMonitor && WindowMode == EWindowMode::Fullscreen))
				{
					return false;
				}
			}
		}
	}
	
	return Super::GetDesiredEnabledState_Implementation();
}

#if WITH_EDITOR
void UVSSettingItemEnabledStateControlWidgetBinder_CommonPresets::EditorRefreshMediator_Implementation()
{
	Super::EditorRefreshMediator_Implementation();
	
	UVSSettingSubsystem* SettingSubsystem = UVSSettingSubsystem::Get();
	if (!GetSettingItem_Native() || !SettingSubsystem) return;
	const FGameplayTag ItemIdentifier = GetSettingItem_Native()->GetItemIdentifier();

	if (ItemIdentifier == VS::Settings::Item::Video::ScreenResolution)
	{
		if (UVSSettingItemBase* SettingItem = GetSettingItem_Native())
		{
			AutoRefreshUpdatedItems.Reset();
			AutoRefreshUpdatedItems.AddTag(VS::Settings::Item::Video::WindowMode);
		}
	}
	else if (ItemIdentifier == VS::Settings::Item::Video::Monitor)
	{
		if (UVSCommonSettingItem* MonitorItem = Cast<UVSCommonSettingItem>(GetSettingItem_Native()))
		{
			AutoRefreshUpdatedItems.Reset();
			AutoRefreshUpdatedItems.AddTag(VS::Settings::Item::Video::WindowMode);
		}
	}
}
#endif
