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

public:
	//~ Begin UVSConsoleVariableSettingItem Interface
	virtual FText ValueStringToText_Implementation(const FString& String) const override;

protected:
	virtual void OnValueUpdated_Implementation() override;
	//~ End UVSConsoleVariableSettingItem Interface

#if WITH_EDITOR
	//~ Begin UVSSettingItem Interface
	virtual bool EditorAllowChangingItemTag_Implementation() const override { return false; }
	virtual bool EditorAllowChangingValueType_Implementation() const override { return false; }
	virtual bool EditorAllowChangingConfigParams_Implementation() const override { return false; }
	virtual bool EditorAllowChangingConsoleVariableName_Implementation() const override { return false; }
#endif
	//~ End UVSSettingItem Interface

public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetVSyncEnabled(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	bool GetVSyncEnabled(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;
};
