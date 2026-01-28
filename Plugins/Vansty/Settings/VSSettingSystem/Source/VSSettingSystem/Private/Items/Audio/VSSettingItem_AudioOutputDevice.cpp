// Copyright VanstySanic. All Rights Reserved.

#include "Items/Audio/VSSettingItem_AudioOutputDevice.h"
#include "Classes/Libraries/VSPlatformLibrary.h"
#include "Items/VSSettingSystemTags.h"

UVSSettingItem_AudioOutputDevice::UVSSettingItem_AudioOutputDevice(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetValueType(EVSCommonSettingValueType::String);

	ItemTag = UGameplayTagsManager::Get().RequestGameplayTagDirectParent(EVSSettingItem::Audio::Device::Output);
	ConfigParams.ConfigFileName = "GameUserSettings";
	ConfigParams.ConfigSection = "VS.Settings.Item.Audio.Device";
	ConfigParams.ConfigKeyName = "AudioOutputDeviceID";
	
	InternalChangeActions = TArray<TEnumAsByte<EVSSettingItemAction::Type>>
	{
		EVSSettingItemAction::Apply,
		EVSSettingItemAction::Confirm,
		EVSSettingItemAction::Save,
	};
	
#if WITH_EDITOR
	EditorChangeActions = TArray<TEnumAsByte<EVSSettingItemAction::Type>>
	{
		EVSSettingItemAction::Apply,
		EVSSettingItemAction::Confirm,
	};
#endif
}

bool UVSSettingItem_AudioOutputDevice::IsValueValid_Implementation() const
{
	if (!Super::IsValueValid_Implementation()) return false;

	const FString& DeviceID = GetStringValue(EVSSettingItemValueSource::System);
	if (!UVSPlatformLibrary::IsValidAudioOutputDeviceID(DeviceID)) return false;
	
	return true;
}

void UVSSettingItem_AudioOutputDevice::Validate_Implementation()
{
	Super::Validate_Implementation();

	if (IsValueValid()) return;
	
	FString DeviceID = GetStringValue(EVSSettingItemValueSource::System);
	if (!UVSPlatformLibrary::IsValidAudioOutputDeviceID(DeviceID))
	{
		DeviceID = UVSPlatformLibrary::GetMainAudioOutputDeviceID();
		if (!DeviceID.IsEmpty())
		{
			SetAudioOutputDeviceID(DeviceID);
		}
	}
}

void UVSSettingItem_AudioOutputDevice::Apply_Implementation()
{
	Super::Apply_Implementation();
	
	const FString& DeviceID = GetStringValue(EVSSettingItemValueSource::System);
	UVSPlatformLibrary::SetMainAudioOutputDeviceByID(DeviceID);
}

FString UVSSettingItem_AudioOutputDevice::GetStringValue_Implementation(const EVSSettingItemValueSource::Type ValueSource) const
{
	switch (ValueSource)
	{
	case EVSSettingItemValueSource::Default:
		return UVSPlatformLibrary::GetSystemDefaultAudioOutputDeviceID();
	
	case EVSSettingItemValueSource::Game:
		return UVSPlatformLibrary::GetMainAudioOutputDeviceID();
		
	default: ;
	}
	
	return Super::GetStringValue_Implementation(ValueSource);
}

FText UVSSettingItem_AudioOutputDevice::ValueStringToText_Implementation(const FString& String) const
{
	const FAudioOutputDeviceInfo& DeviceInfo = UVSPlatformLibrary::GetAudioOutputDeviceInfoByID(String);
	if (!DeviceInfo.DeviceId.IsEmpty())
	{
		return FText::FromString(DeviceInfo.Name);
	}
	
	return Super::ValueStringToText_Implementation(String);
}

void UVSSettingItem_AudioOutputDevice::SetAudioOutputDeviceID(const FString& MonitorID)
{
	SetStringValue(MonitorID);	
}

FString UVSSettingItem_AudioOutputDevice::GetAudioOutputDeviceID(EVSSettingItemValueSource::Type ValueSource) const
{
	return GetStringValue(ValueSource);
}
