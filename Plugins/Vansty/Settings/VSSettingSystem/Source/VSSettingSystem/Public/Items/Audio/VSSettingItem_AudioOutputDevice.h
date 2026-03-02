// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSCommonSettingItem.h"
#include "VSSettingItem_AudioOutputDevice.generated.h"

/**
 * Audio output-device setting item.
 *
 * Stores and applies the selected output device ID.
 */
UCLASS(DisplayName = "VS.SettingSystem.Item.Audio.Device.Output")
class VSSETTINGSYSTEM_API UVSSettingItem_AudioOutputDevice : public UVSCommonSettingItem
{
	GENERATED_UCLASS_BODY()

protected:
	//~ Begin UVSSettingItem Interface
	virtual bool IsValueValid_Implementation() const override;
	virtual void Validate_Implementation() override;
	virtual void Apply_Implementation() override;
	virtual FString GetStringValue_Implementation(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const override;
	virtual FText ValueStringToText_Implementation(const FString& String) const override;

#if WITH_EDITOR
	virtual bool EditorAllowChangingItemTag_Implementation() const override { return false; }
	virtual bool EditorAllowChangingValueType_Implementation() const override { return false; }
#endif
	//~ End UVSSettingItem Interface

public:
	/** Sets the target audio output device ID. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetAudioOutputDeviceID(const FString& DeviceID);

	/** Returns the audio output device ID for the requested source. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	FString GetAudioOutputDeviceID(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;
};
