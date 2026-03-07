// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSConsoleVariableSettingItem.h"
#include "VSSettingItem_MotionBlur.generated.h"

/**
 * Motion blur quality setting item.
 *
 * Exposes quality-level control and convenience enabled-state query.
 */
UCLASS(DisplayName = "VS.SettingSystem.Item.Graphics.MotionBlur")
class VSSETTINGSYSTEM_API UVSSettingItem_MotionBlur : public UVSConsoleVariableSettingItem
{
	GENERATED_UCLASS_BODY()

protected:
	//~ Begin UVSSettingItem Interface
	virtual bool IsValueValid_Implementation() const override;
	virtual void Validate_Implementation() override;
	virtual int32 GetIntegerValue_Implementation(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const override;
	virtual FText ValueStringToText_Implementation(const FString& String) const override;
	
#if WITH_EDITOR
	virtual bool EditorAllowChangingItemIdentifier_Implementation() const override { return false; }
	virtual bool EditorAllowChangingValueType_Implementation() const override { return false; }
	virtual bool EditorAllowChangingConfigParams_Implementation() const override { return false; }
	virtual bool EditorAllowChangingConsoleVariableName_Implementation() const override { return false; }
#endif
	//~ End UVSSettingItem Interface

public:
	/** Sets motion blur quality level (typically 0-4). */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetMotionBlurQualityLevel(int32 QualityLevel);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	int32 GetMotionBlurQualityLevel(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;
		
	UFUNCTION(BlueprintCallable, Category = "Settings")
	bool GetMotionBlurEnabled(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;
};
