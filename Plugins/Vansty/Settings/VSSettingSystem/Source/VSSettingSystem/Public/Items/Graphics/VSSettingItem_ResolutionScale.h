// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSConsoleVariableSettingItem.h"
#include "VSSettingItem_ResolutionScale.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "VS.SettingSystem.Item.Scalability.ResolutionScale")
class VSSETTINGSYSTEM_API UVSSettingItem_ResolutionScale : public UVSConsoleVariableSettingItem
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UVSSettingItem Interface
	virtual bool IsValueValid_Implementation() const override;
	virtual void Validate_Implementation() override;
	virtual float GetFloatValue_Implementation(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const override;
	//~ End UVSSettingItem Interface

protected:
	//~ Begin UVSSettingItem Interface
#if WITH_EDITOR
	virtual bool AllowEditorChangingItemTag_Implementation() const override { return false; }
	virtual bool AllowEditorChangingValueType_Implementation() const override { return false; }
	virtual bool AllowEditorChangingConfigParams_Implementation() const override { return false; }
	virtual bool AllowEditorChangingConsoleVariableName_Implementation() const override { return false; }
#endif
	//~ End UVSSettingItem Interface

public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetResolutionScale(float InResolutionScale);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	float GetResolutionScale(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;

};
