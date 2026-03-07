// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSCommonSettingItem.h"
#include "VSSettingItem_WindowMode.generated.h"

/**
 * Window-mode setting item.
 *
 * Tracks and applies `EWindowMode::Type` while reacting to viewport changes.
 */
UCLASS(DisplayName = "VS.Settings.Item.Video.WindowMode")
class VSSETTINGSYSTEM_API UVSSettingItem_WindowMode : public UVSCommonSettingItem
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UObject Interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~ End UObject Interface

	//~ Begin UVSSettingItem Interface
protected:
	virtual void Initialize_Implementation() override;
	virtual void Uninitialize_Implementation() override;

	virtual void Apply_Implementation() override;
	virtual void Confirm_Implementation() override;
	virtual void Save_Implementation() override;
	virtual bool IsValueValid_Implementation() const override;
	virtual void Validate_Implementation() override;
	virtual int32 GetIntegerValue_Implementation(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const override;
	virtual FText ValueStringToText_Implementation(const FString& String) const override;
	
	virtual void OnValueUpdated_Implementation() override;
	
#if WITH_EDITOR
	virtual bool EditorAllowChangingItemIdentifier_Implementation() const override { return false; }
	virtual bool EditorAllowChangingValueType_Implementation() const override { return false; }
	virtual bool EditorAllowChangingConfigParams_Implementation() const override { return false; }
	virtual bool EditorAllowChangingEditorPreviewValue_Implementation() const override { return false; }
#endif
	//~ End UVSSettingItem Interface

public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetWindowMode(EWindowMode::Type InWindowMode);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	EWindowMode::Type GetWindowMode(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;

private:
	/** Handles external viewport resize changes to trigger external-change actions. */
	void OnViewportResized(FViewport* Viewport, uint32 Value);

protected:
	/** Whether command-line window-mode overrides should be honored when applying. */
	UPROPERTY(EditAnywhere, Category = "Settings")
	uint8 bCheckForCommandLineOverrides : 1;
	
private:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, DisplayName = "Preview Window Mode", Category = "Settings", Transient)
	TEnumAsByte<EWindowMode::Type> EditorPreviewWindowMode;
#endif
};
