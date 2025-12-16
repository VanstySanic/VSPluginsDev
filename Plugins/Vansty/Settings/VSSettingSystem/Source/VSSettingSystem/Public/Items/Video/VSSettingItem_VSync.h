// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSConsoleVariableSettingItem.h"
#include "VSSettingItem_VSync.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "VS.SettingSystem.Item.Video.VSync")
class VSSETTINGSYSTEM_API UVSSettingItem_VSync : public UVSConsoleVariableSettingItem
{
	GENERATED_UCLASS_BODY()

protected:
	//~ Begin UVSSettingItem Interface
	virtual void OnValueUpdated_Implementation() override;
	//~ End UVSSettingItem Interface

#if WITH_EDITOR
	//~ Begin UVSSettingItem Interface
	virtual bool AllowEditorChangingItemTag_Implementation() const override { return false; }
	virtual bool AllowEditorChangingValueType_Implementation() const override { return false; }
	virtual bool AllowEditorChangingConfigParams_Implementation() const override { return false; }
	virtual bool AllowEditorChangingConsoleVariableName_Implementation() const override { return false; }
#endif
	//~ End UVSSettingItem Interface

public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetVSyncEnabled(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	bool GetVSyncEnabled(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;
};
