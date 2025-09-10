// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSSettingItemBase.h"
#include "Items/VSSettingItemBase_Float.h"
#include "VSSettingItem_ResolutionScale.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Settings.Item.Scalability.ResolutionScale")
class VSSETTINGSYSTEM_API UVSSettingItem_ResolutionScale : public UVSSettingItemBase_Float
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void Load_Implementation() override;
	virtual void Apply_Implementation() override;
	virtual void Confirm_Implementation() override;
	virtual void Save_Implementation() override;
	
	virtual float GetValue_Implementation(EVSSettingItemValueSource::Type ValueType = EVSSettingItemValueSource::Settings) const override;
	virtual void SetValue_Implementation(float InValue) override;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetResolutionScale(float InResolutionScale);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	float GetResolutionScale(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::Settings) const;

private:
	float ResolutionScale = 100.f;
	float LastConfirmedResolutionScale = 100.f;
};
