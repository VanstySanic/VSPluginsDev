// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSSettingItemBase.h"
#include "VSMutableFloatSettingItem.generated.h"

/**
 * 
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
	UFUNCTION(BlueprintCallable, Category = "Settings")
	float GetValue(EVSSettingItemValueSource::Type Source = EVSSettingItemValueSource::System) const;

	/** Get the float value that concerns with no mutation. */
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	float GetNonMutedValue(EVSSettingItemValueSource::Type Source = EVSSettingItemValueSource::System) const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings")
	bool GetIsMuted(EVSSettingItemValueSource::Type Source = EVSSettingItemValueSource::System) const;

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetValue(float Value);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetIsMuted(bool bMuted);

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
	UPROPERTY(EditAnywhere, Category = "Display")
	FText DisplayTextFormat;

	UPROPERTY(EditAnywhere, Category = "Display")
	FIntPoint DisplayFractionDigitRange = FIntPoint(1, 324);

	UPROPERTY(EditAnywhere, Category = "Display")
	float DisplayValueMultiplier = 1.f;
	
	/** Used if not empty. */
	UPROPERTY(EditAnywhere, Category = "Display")
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
	float CurrentValue = 0.f;
	float ConfirmedValue = 0.f;
};