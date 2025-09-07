// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSSettingItemBase_Float.h"
#include "VSSettingItem_Volume.generated.h"

/**
 * 
 */
UCLASS(Abstract, DisplayName = "Settings.Item.Audio.Volume")
class VSSETTINGSYSTEM_API UVSSettingItem_Volume : public UVSSettingItemBase_Float
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void Load_Implementation() override;
	virtual void Apply_Implementation() override;
	virtual void Confirm_Implementation() override;
	virtual void Save_Implementation() override;

	virtual void SetValue_Implementation(float InValue) override;
	virtual float GetValue_Implementation(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::Settings) const override;

public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetVolume(float InVolume);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	float GetVolume(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::Settings) const;

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetForceMute(bool bInForceMute);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	bool GetForceMute(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::Settings) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TObjectPtr<USoundMix> SoundMix;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TObjectPtr<USoundClass> SoundClass;
	
private:
	float Volume = 0.f;
	float LastConfirmedVolume = 0.f;
	bool bForceMute = false;
	int32 LastConfirmedForceMute = 0.f;
};
