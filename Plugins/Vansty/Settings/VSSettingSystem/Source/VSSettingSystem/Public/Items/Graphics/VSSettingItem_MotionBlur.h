// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSConsoleVariableSettingItem.h"
#include "VSSettingItem_MotionBlur.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "VS.SettingSystem.Item.Graphics.MotionBlur")
class VSSETTINGSYSTEM_API UVSSettingItem_MotionBlur : public UVSConsoleVariableSettingItem
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UVSSettingItem Interface
	virtual bool IsValueValid_Implementation() const override;
	virtual void Validate_Implementation() override;
	virtual int32 GetIntegerValue_Implementation(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const override;
	
protected:
#if WITH_EDITOR
	virtual bool EditorAllowChangingItemTag_Implementation() const override { return false; }
	virtual bool EditorAllowChangingValueType_Implementation() const override { return false; }
	virtual bool EditorAllowChangingConfigParams_Implementation() const override { return false; }
	virtual bool EditorAllowChangingConsoleVariableName_Implementation() const override { return false; }
#endif
	//~ End UVSSettingItem Interface

public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetMotionBlurQualityLevel(int32 QualityLevel);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	int32 GetMotionBlurQualityLevel(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;
		
	UFUNCTION(BlueprintCallable, Category = "Settings")
	bool GetMotionBlurEnabled(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;
};
