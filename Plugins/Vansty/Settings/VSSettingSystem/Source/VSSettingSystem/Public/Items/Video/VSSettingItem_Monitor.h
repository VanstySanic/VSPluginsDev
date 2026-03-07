// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSCommonSettingItem.h"
#include "VSSettingItem_Monitor.generated.h"

/**
 * Monitor selection setting item.
 *
 * Stores display monitor ID and applies monitor targeting for fullscreen modes.
 */
UCLASS(DisplayName = "VS.SettingSystem.Item.Video.Monitor")
class VSSETTINGSYSTEM_API UVSSettingItem_Monitor : public UVSCommonSettingItem
{
	GENERATED_UCLASS_BODY()

protected:
	//~ Begin UVSSettingItem Interface
	virtual void Apply_Implementation() override;
	virtual bool IsValueValid_Implementation() const override;
	virtual void Validate_Implementation() override;
	virtual FString GetStringValue_Implementation(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const override;
	virtual FText ValueStringToText_Implementation(const FString& String) const override;
	
#if WITH_EDITOR
	virtual bool EditorAllowChangingItemIdentifier_Implementation() const override { return false; }
	virtual bool EditorAllowChangingValueType_Implementation() const override { return false; }
	virtual bool EditorAllowChangingConfigParams_Implementation() const override { return false; }
	virtual bool EditorAllowChangingEditorPreviewValue_Implementation() const override { return false; }
#endif
	//~ End UVSSettingItem Interface

public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetMonitorID(const FString& MonitorID);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	FString GetMonitorID(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;

public:
	/** Whether this setting should override desired fullscreen monitor selection. */
	UPROPERTY(EditAnywhere, Category = "Settings")
	uint8 bOverrideDesiredFullscreenMonitor : 1;
};
