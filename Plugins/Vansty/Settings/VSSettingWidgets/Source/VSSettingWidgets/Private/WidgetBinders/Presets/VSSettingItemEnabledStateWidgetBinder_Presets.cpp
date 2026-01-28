// Copyright VanstySanic. All Rights Reserved.

#include "WidgetBinders/Presets/VSSettingItemEnabledStateWidgetBinder_Presets.h"
#include "VSSettingSubsystem.h"
#include "Items/VSCommonSettingItem.h"
#include "Items/VSSettingItem.h"
#include "Items/VSSettingSystemTags.h"
#include "Items/Video/VSSettingItem_Monitor.h"
#include "Items/Video/VSSettingItem_ScreenResolution.h"
#include "Items/Video/VSSettingItem_WindowMode.h"

UVSSettingItemEnabledStateWidgetBinder_Presets::UVSSettingItemEnabledStateWidgetBinder_Presets(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UVSSettingItemEnabledStateWidgetBinder_Presets::OnAnySettingItemUpdated_Implementation(UVSSettingItem* SettingItem)
{
	Super::OnAnySettingItemUpdated_Implementation(SettingItem);

	if (AutoRefreshUpdatedItems.HasTag(SettingItem->GetItemTag()))
	{
		RefreshEnabledState();
	}
}

bool UVSSettingItemEnabledStateWidgetBinder_Presets::GetDesiredEnabledState_Implementation()
{
	UVSSettingSubsystem* SettingSubsystem = UVSSettingSubsystem::Get();
	if (!GetSettingItem_Native() || !SettingSubsystem) return Super::GetDesiredEnabledState_Implementation();
	const FGameplayTag ItemTag = GetSettingItem_Native()->GetItemTag();

	if (ItemTag == EVSSettingItem::Video::ScreenResolution)
	{
		if (UVSSettingItem_ScreenResolution* ScreenResolutionItem = Cast<UVSSettingItem_ScreenResolution>(GetSettingItem_Native()))
		{
			if (UVSSettingItem_WindowMode* WindowModeItem = Cast<UVSSettingItem_WindowMode>(SettingSubsystem->GetSettingItemByTag(EVSSettingItem::Video::WindowMode)))
			{
				if (WindowModeItem->GetWindowMode(EVSSettingItemValueSource::System) == EWindowMode::WindowedFullscreen && ScreenResolutionItem->DisableInWindowedFullscreenMode)
				{
					return false;
				}
			}
		}
	}
	else if (ItemTag == EVSSettingItem::Video::Monitor)
	{
		if (UVSSettingItem_Monitor* MonitorItem = Cast<UVSSettingItem_Monitor>(GetSettingItem_Native()))
		{
			if (UVSSettingItem_WindowMode* WindowModeItem = Cast<UVSSettingItem_WindowMode>(SettingSubsystem->GetSettingItemByTag(EVSSettingItem::Video::WindowMode)))
			{
				if (!MonitorItem->SupportedWindowModes.Contains(WindowModeItem->GetWindowMode(EVSSettingItemValueSource::System)))
				{
					return false;
				}
			}
		}
	}
	
	return Super::GetDesiredEnabledState_Implementation();
}

#if WITH_EDITOR
void UVSSettingItemEnabledStateWidgetBinder_Presets::EditorRefreshMediator_Implementation()
{
	Super::EditorRefreshMediator_Implementation();
	
	UVSSettingSubsystem* SettingSubsystem = UVSSettingSubsystem::Get();
	if (!GetSettingItem_Native() || !SettingSubsystem) return;
	const FGameplayTag& ItemTag = GetSettingItem_Native()->GetItemTag();

	if (ItemTag == EVSSettingItem::Video::ScreenResolution)
	{
		if (UVSSettingItem* SettingItem = GetSettingItem_Native())
		{
			AutoRefreshUpdatedItems.Reset();
			AutoRefreshUpdatedItems.AddTag(EVSSettingItem::Video::WindowMode);
		}
	}
	else if (ItemTag == EVSSettingItem::Video::Monitor)
	{
		if (UVSCommonSettingItem* MonitorItem = Cast<UVSCommonSettingItem>(GetSettingItem_Native()))
		{
			AutoRefreshUpdatedItems.Reset();
			AutoRefreshUpdatedItems.AddTag(EVSSettingItem::Video::WindowMode);
		}
	}
}
#endif
