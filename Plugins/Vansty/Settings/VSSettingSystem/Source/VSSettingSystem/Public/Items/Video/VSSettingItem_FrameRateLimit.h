// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSConsoleVariableSettingItem.h"
#include "VSSettingItem_FrameRateLimit.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "VS.SettingSystem.Item.Video.FrameRateLimit")
class VSSETTINGSYSTEM_API UVSSettingItem_FrameRateLimit : public UVSConsoleVariableSettingItem
{
	GENERATED_UCLASS_BODY()

public:

protected:
	//~ Begin UVSSettingItem Interface
	virtual bool IsValueValid_Implementation() const override;
	virtual void Validate_Implementation() override;
	virtual void OnValueUpdated_Implementation() override;

#if WITH_EDITOR
	virtual bool AllowEditorChangingItemTag_Implementation() const override { return false; }
	virtual bool AllowEditorChangingValueType_Implementation() const override { return false; }
	virtual bool AllowEditorChangingConfigParams_Implementation() const override { return false; }
	virtual bool AllowEditorChangingConsoleVariableName_Implementation() const override { return false; }
#endif
	//~ End UVSSettingItem Interface

public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetFrameRateLimit(float InLimit);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	float GetFrameRateLimit(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;
};
