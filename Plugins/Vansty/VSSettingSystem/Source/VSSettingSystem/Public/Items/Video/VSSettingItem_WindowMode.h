// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSSettingItemBase.h"
#include "Items/VSSettingItemBase_Integer.h"
#include "VSSettingItem_WindowMode.generated.h"

enum class ERotatorDirection : uint8;

/**
 * 
 */
UCLASS(DisplayName = "Settings.Item.Video.WindowMode")
class VSSETTINGSYSTEM_API UVSSettingItem_WindowMode : public UVSSettingItemBase_Integer
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void Load_Implementation() override;
	virtual void Validate_Implementation() override;
	virtual void Apply_Implementation() override;
	virtual void Confirm_Implementation() override;
	virtual void Save_Implementation() override;

	virtual void SetValue_Implementation(int32 InValue) override;
	virtual int32 GetValue_Implementation(EVSSettingItemValueSource::Type ValueType = EVSSettingItemValueSource::Settings) const override;

public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetWindowMode(EWindowMode::Type InWindowMode);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	EWindowMode::Type GetWindowMode(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::Settings) const;
};
