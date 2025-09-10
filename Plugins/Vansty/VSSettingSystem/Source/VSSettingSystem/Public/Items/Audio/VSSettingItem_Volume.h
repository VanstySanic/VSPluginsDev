// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSSettingItemBase_Float.h"
#include "VSSettingItem_Volume.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Settings.Item.Audio.Volume")
class VSSETTINGSYSTEM_API UVSSettingItem_Volume : public UVSSettingItemBase_Float
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void Initialize_Implementation() override;
	virtual void Uninitialize_Implementation() override;
	virtual void Load_Implementation() override;
	virtual void Apply_Implementation() override;
	virtual void Confirm_Implementation() override;
	virtual void Save_Implementation() override;
	virtual void OnItemValueUpdated_Implementation() override;

	virtual void SetValue_Implementation(float InValue) override;
	virtual float GetValue_Implementation(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::Settings) const override;

	virtual void BindWidgetInternal_Implementation(UWidget* Widget, FName TypeName = NAME_None) override;
	virtual void UnbindWidgetInternal_Implementation(UWidget* Widget, FName TypeName) override;

private:
	UFUNCTION()
	void OnMuteCheckBoxCheckStateChanged(bool bIsChecked);
	
public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetVolume(float InVolume);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	float GetVolume(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::Settings) const;

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetForceMute(bool bInForceMute);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	bool GetForceMute(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::Settings) const;

private:
	void ApplyToAudioDevice(FAudioDevice* AudioDevice);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Volume")
	TObjectPtr<USoundMix> SoundMix;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Volume")
	TObjectPtr<USoundClass> SoundClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Volume")
	FText MutedText;

private:
	float Volume = 0.f;
	float LastConfirmedVolume = 0.f;
	bool bForceMute = false;
	bool bAppliedForceMute = false;
	bool bLastConfirmedForceMute = false;
	
	TArray<TWeakObjectPtr<UWidget>> MutedWidgets;
	FDelegateHandle OnAudioDeviceCreatedDelegateHandle;
};
