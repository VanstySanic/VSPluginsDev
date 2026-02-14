// Copyright VanstySanic. All Rights Reserved.

#include "Mediator/Binders/Presets/VSSettingItemEnabledStateControlWidgetBinder_CommonPresets.h"
#include "VSSettingSubsystem.h"
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

	if (AutoRefreshUpdatedItems.HasTag(SettingItem->GetItemTag()))
	{
		RefreshEnabledState();
	}
}

bool UVSSettingItemEnabledStateControlWidgetBinder_CommonPresets::GetDesiredEnabledState_Implementation()
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
	const FGameplayTag ItemTag = GetSettingItem_Native()->GetItemTag();

	if (ItemTag == EVSSettingItem::Video::ScreenResolution)
	{
		if (UVSSettingItemBase* SettingItem = GetSettingItem_Native())
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
