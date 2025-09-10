// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSSettingItemBase_Boolean.h"
#include "VSSettingItem_VSync.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Settings.Item.Video.VSync")
class VSSETTINGSYSTEM_API UVSSettingItem_VSync : public UVSSettingItemBase_Boolean
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void Load_Implementation() override;
	virtual void Apply_Implementation() override;
	virtual void Confirm_Implementation() override;
	virtual void Save_Implementation() override;

	virtual void SetValue_Implementation(bool bInValue) override;
	virtual bool GetValue_Implementation(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::Settings) const override;

public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetVSyncEnabled(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	bool GetVSyncEnabled(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::Settings) const;

private:
	bool LastConfirmedVSyncEnabled = false;
};
