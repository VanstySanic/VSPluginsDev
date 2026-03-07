// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSSettingItemBase.h"
#include "VSMutableFloatSettingItem.generated.h"

/**
 * Base setting item for mutable float values with optional mute state.
 *
 * Maintains system/confirmed float values and a parallel muted flag, and
 * provides display-format helpers for UI presentation.
 */
UCLASS(Abstract, DisplayName = "VS.Settings.Item.MutableFloat")
class VSSETTINGSYSTEM_API UVSMutableFloatSettingItem : public UVSSettingItemBase
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UObject Interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~ End UObject Interface

protected:
	//~ Begin UVSSettingItem Interface
	virtual bool IsValueValid_Implementation() const override;
	virtual void Validate_Implementation() override;
	virtual void SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) override;
	virtual bool EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const override;
	virtual void Load_Implementation() override;
	virtual void Save_Implementation() override;
	virtual void Confirm_Implementation() override;

	virtual void OnValueUpdated_Implementation() override;
	//~ End UVSSettingItem Interface

public:
	/** Returns value with mute-state substitution applied. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	float GetValue(EVSSettingItemValueSource::Type Source = EVSSettingItemValueSource::System) const;

	/** Returns the underlying float value without mute-state substitution. */
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	float GetNonMutedValue(EVSSettingItemValueSource::Type Source = EVSSettingItemValueSource::System) const;
	
	/** Returns whether this source is currently muted. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings")
	bool GetIsMuted(EVSSettingItemValueSource::Type Source = EVSSettingItemValueSource::System) const;

	/** Sets the non-muted value component. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetValue(float Value);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetIsMuted(bool bMuted);

	/** Formats value and mute state for display text. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	FText ValueMuteToText(float Value, bool bIsMuted);

protected:
#if WITH_EDITOR
	/** Whether ConfigParams can be edited in the editor for this item. */
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	bool EditorAllowChangingConfigParams() const;
	
	/** Whether EditorPreviewValue can be edited in the editor for this item. */
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	bool EditorAllowChangingEditorPreviewValue() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	bool EditorAllowChangingEditorPreviewMuteState() const;
#endif
	
protected:
	UPROPERTY(EditAnywhere, Category = "Settings")
	float MuteStateValue = 0.f;
	
public:
	UPROPERTY(EditAnywhere, Category = "Settings|Display")
	FText DisplayTextFormat;

	UPROPERTY(EditAnywhere, Category = "Settings|Display")
	FIntPoint DisplayFractionDigitRange = FIntPoint(1, 324);

	UPROPERTY(EditAnywhere, Category = "Settings|Display")
	float DisplayValueMultiplier = 1.f;
	
	/** Optional text used when muted. If empty, numeric text formatting is used instead. */
	UPROPERTY(EditAnywhere, Category = "Settings|Display")
	FText DisplayMutedText;

private:
	/**
	 * Preview value that can be modified in editor.
	 * @remarks This is transient and will not be saved or loaded.
	 */
	UPROPERTY(EditAnywhere, DisplayName = "Preview Value", Category = "Settings", meta = (EditCondition = "EditorAllowChangingEditorPreviewValue()"))
	float EditorPreviewValue;
	
	UPROPERTY(EditAnywhere, DisplayName = "Preview Mute State", Category = "Settings", meta = (EditCondition = "EditorAllowChangingEditorPreviewMuteState()"))
	uint8 bEditorPreviewMuteState : 1;
	
	UPROPERTY(VisibleAnywhere, DisplayName = "Preview Text", Category = "Settings", Transient)
	FText EditorPreviewText;
	
	uint8 bCurrentIsMuted : 1;
	uint8 bConfirmedIsMuted : 1;
	/** System non-muted value. */
	float CurrentValue = 0.f;
	/** Confirmed non-muted value. */
	float ConfirmedValue = 0.f;
};
