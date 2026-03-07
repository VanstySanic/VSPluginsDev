// Copyright VanstySanic. All Rights Reserved.

#include "Items/Video/VSSettingItem_Monitor.h"
#include "Classes/Libraries/VSPlatformLibrary.h"
#include "Items/VSSettingSystemTags.h"

UVSSettingItem_Monitor::UVSSettingItem_Monitor(const FObjectInitializer& FObjectInitializer)
	: Super(FObjectInitializer)
{
	SetValueType(EVSCommonSettingValueType::String);
	
	ItemIdentifier = EVSSettingItem::Video::Monitor;
	ItemInfo.ItemTags.AddTag(EVSSettingItem::Video::Monitor.GetTag().RequestDirectParent());
	ItemInfo.DisplayName = NSLOCTEXT("VS.SettingSystem.Item.Video.Monitor", "DisplayName", "Monitor");
	ConfigSettings.Section = "VS.Settings.Item.Video";
	ConfigSettings.PrimaryKey = "Monitor";
}

void UVSSettingItem_Monitor::Apply_Implementation()
{
	Super::Apply_Implementation();
	
	EWindowMode::Type WindowMode = GSystemResolution.WindowMode;
    if (auto Window = UVSPlatformLibrary::GetGameWindow())
    {
    	WindowMode = Window->GetWindowMode();
    }

	if (!bOverrideDesiredFullscreenMonitor && WindowMode == EWindowMode::Fullscreen) return;
	
	const FString MonitorID = GetMonitorID(EVSSettingItemValueSource::System);
	const FVSMonitorInfo MonitorInfo = UVSPlatformLibrary::GetMonitorInfoByID(MonitorID);
	if (!MonitorInfo.ID.IsEmpty())
	{

		
		const FVector2D WindowCenterPos = UVSPlatformLibrary::GetGameWindowCenterPosition(false);
		const FString WindowMonitorID = UVSPlatformLibrary::GetMonitorIDByPosition(WindowCenterPos);
		const FMonitorInfo WindowMonitorInfo = UVSPlatformLibrary::GetNativeMonitorInfoByID(WindowMonitorID);
		if (!MonitorID.IsEmpty() && WindowMonitorInfo.ID != MonitorID)
		{
			if (WindowMode == EWindowMode::Windowed)
			{
				UVSPlatformLibrary::SetGameWindowCenteredAtMonitor(MonitorID);
			}
			else
			{
				UVSPlatformLibrary::SetGameWindowPosition(MonitorInfo.GetRootPosition());
			}

			if (bOverrideDesiredFullscreenMonitor)
			{
				UVSPlatformLibrary::SetDesiredFullscreenMonitorID(MonitorID);
			}
		}
	}
}

bool UVSSettingItem_Monitor::IsValueValid_Implementation() const
{
	const FString MonitorID = GetMonitorID(EVSSettingItemValueSource::System);
	if (!UVSPlatformLibrary::IsValidMonitorID(MonitorID)) return false;
	
	return Super::IsValueValid_Implementation();
}

void UVSSettingItem_Monitor::Validate_Implementation()
{
	Super::Validate_Implementation();

	if (IsValueValid()) return;

	const FString MonitorID = GetMonitorID(EVSSettingItemValueSource::System);
	if (!UVSPlatformLibrary::IsValidMonitorID(MonitorID) && !UVSPlatformLibrary::GetPrimaryMonitorID().IsEmpty())
	{
		SetStringValue(UVSPlatformLibrary::GetPrimaryMonitorID());
	}
}

FString UVSSettingItem_Monitor::GetStringValue_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	if (ValueType == EVSCommonSettingValueType::String)
	{
		switch (ValueSource)
		{
		case EVSSettingItemValueSource::Default:
			return UVSPlatformLibrary::GetPrimaryMonitorID();
			
		case EVSSettingItemValueSource::Game:
			if (auto Window = UVSPlatformLibrary::GetGameWindow())
			{
				return UVSPlatformLibrary::GetMonitorIDByPosition(Window->GetPositionInScreen());
			}
			return UVSPlatformLibrary::GetPrimaryMonitorID();
			
		default: ;
		}
	}
		
	return Super::GetStringValue_Implementation(ValueSource);
}

FText UVSSettingItem_Monitor::ValueStringToText_Implementation(const FString& String) const
{
	const FMonitorInfo MonitorInfo = UVSPlatformLibrary::GetNativeMonitorInfoByID(String);
	FString MonitorName = MonitorInfo.Name;
	int32 SameNameMonitorNum = 0;
	int32 SameNameMonitorIndex = 0;
	
	for (const FMonitorInfo& AllMonitorInfo : UVSPlatformLibrary::GetAvailableNativeMonitorInfos())
	{
		if (MonitorName == AllMonitorInfo.Name)
		{
			SameNameMonitorNum++;
		}
	}
	
	for (const FMonitorInfo& AllMonitorInfo : UVSPlatformLibrary::GetAvailableNativeMonitorInfos())
	{
		if (MonitorName == AllMonitorInfo.Name)
		{
			if (AllMonitorInfo.ID != String)
			{
				SameNameMonitorIndex++;
			}
			else
			{
				break;
			}
		}
	}

	if (SameNameMonitorNum > 1)
	{
		MonitorName += " " + FString::FromInt(SameNameMonitorIndex + 1);
	}
	
	return FText::FromString(MonitorName);
}

void UVSSettingItem_Monitor::SetMonitorID(const FString& MonitorID)
{
	SetStringValue(MonitorID);
}

FString UVSSettingItem_Monitor::GetMonitorID(EVSSettingItemValueSource::Type ValueSource) const
{
	return GetStringValue(ValueSource);
}
