// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSCommonSettingItem.h"
#include "VSSettingItem_AudioOutputDevice.generated.h"

/**
 * 
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
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetAudioOutputDeviceID(const FString& MonitorID);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	FString GetAudioOutputDeviceID(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;
};
