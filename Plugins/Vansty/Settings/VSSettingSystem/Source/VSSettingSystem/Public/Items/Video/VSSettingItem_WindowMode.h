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
	virtual void PostLoad() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~ End UObject Interface
	
protected:
	//~ Begin UVSSettingItem Interface
	virtual void Apply_Implementation() override;
	virtual void Confirm_Implementation() override;
	virtual void Save_Implementation() override;
	virtual bool IsValueValid_Implementation() const override;
	virtual void Validate_Implementation() override;
	virtual void OnValueUpdated_Implementation() override;
	virtual int32 GetIntegerValue_Implementation(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const override;

#if WITH_EDITOR
	virtual bool AllowEditorChangingItemTag_Implementation() const override { return false; }
	virtual bool AllowEditorChangingValueType_Implementation() const override { return false; }
	virtual bool AllowEditorChangingConfigParams_Implementation() const override { return false; }
	virtual bool AllowEditorChangingEditorPreviewValue_Implementation() const override { return false; }
#endif
	//~ End UVSSettingItem Interface

public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetWindowMode(EWindowMode::Type InWindowMode);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	EWindowMode::Type GetWindowMode(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const;
	
private:
#if WITH_EDITORONLY_DATA
	/** Preview window mode that can be modified in editor. */
	UPROPERTY(EditAnywhere, DisplayName = "Preview Window Mode", Category = "Settings", Transient)
	TEnumAsByte<EWindowMode::Type> EditorPreviewWindowMode;
#endif
};
