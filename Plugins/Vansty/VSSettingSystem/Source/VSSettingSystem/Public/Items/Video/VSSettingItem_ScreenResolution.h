// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSSettingItemBase.h"
#include "VSSettingItem_ScreenResolution.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Settings.Item.ScreenResolution")
class VSSETTINGSYSTEM_API UVSSettingItem_ScreenResolution : public UVSSettingItemBase
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void Apply_Implementation() override;
	virtual void Confirm_Implementation() override;
	virtual void Save_Implementation() override;
	virtual void SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) override;
	virtual bool EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const override;

public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetScreenResolution(const FIntPoint& Resolution);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	FIntPoint GetScreenResolution(EVSSettingItemValueSource::Type ValueSource) const;
};
