// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSConsoleVariableSettingItem.h"
#include "VSSettingItem_FrameRateLimit.generated.h"

/**
 * Frame-rate limit setting item.
 *
 * Controls max frame rate and formats special values (for example no limit).
 */
UCLASS(DisplayName = "VS.Settings.Item.Video.FrameRateLimit")
class VSSETTINGSYSTEM_API UVSSettingItem_FrameRateLimit : public UVSConsoleVariableSettingItem
{
	GENERATED_UCLASS_BODY()

protected:
	//~ Begin UVSSettingItem Interface
	virtual bool IsValueValid_Implementation() const override;
	virtual void Validate_Implementation() override;
	virtual FText ValueStringToText_Implementation(const FString& String) const override;

#if WITH_EDITOR
	virtual bool EditorAllowChangingItemIdentifier_Implementation() const override { return false; }
	virtual bool EditorAllowChangingValueType_Implementation() const override { return false; }
	virtual bool EditorAllowChangingConfigParams_Implementation() const override { return false; }
	virtual bool EditorAllowChangingConsoleVariableName_Implementation() const override { return false; }
#endif

protected:
	virtual void OnValueUpdated_Implementation() override;
	//~ End UVSSettingItem Interface

public:
	/** Sets frame-rate cap value (0 means no limit). */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetFrameRateLimit(float InLimit);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	float GetFrameRateLimit(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;
};
