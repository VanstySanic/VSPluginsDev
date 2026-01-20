// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSCommonSettingItem.h"
#include "VSSettingItem_WindowMode.generated.h"

/**
 * 
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

public:
	virtual void Apply_Implementation() override;
	virtual void Confirm_Implementation() override;
	virtual void Save_Implementation() override;
	virtual bool IsValueValid_Implementation() const override;
	virtual void Validate_Implementation() override;
	virtual int32 GetIntegerValue_Implementation(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const override;
	virtual FText ValueStringToText_Implementation(const FString& String) const override;
	
protected:
	virtual void OnValueUpdated_Implementation() override;
#if WITH_EDITOR
	virtual void EditorPostInitialized_Implementation() override;
	virtual bool EditorAllowChangingItemTag_Implementation() const override { return false; }
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
	void OnViewportResized(FViewport* Viewport, uint32 Value);

protected:
	UPROPERTY(EditAnywhere, Category = "Settings")
	uint8 bCheckForCommandLineOverrides : 1;
	
private:
#if WITH_EDITORONLY_DATA
	/** Preview window mode that can be modified in editor. */
	UPROPERTY(EditAnywhere, DisplayName = "Preview Window Mode", Category = "Settings", Transient)
	TEnumAsByte<EWindowMode::Type> EditorPreviewWindowMode;
#endif
};
