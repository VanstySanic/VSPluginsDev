// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSSettingItemBase_Float.h"
#include "VSSettingItem_FrameRateLimit.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Settings.Item.Video.FrameRateLimit")
class VSSETTINGSYSTEM_API UVSSettingItem_FrameRateLimit : public UVSSettingItemBase_Float
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
	void SetFrameRateLimit(float InLimit);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	float GetFrameRateLimit(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::Settings) const;

private:
	float LastConfirmedFrameRateLimit = 0.f;
};
