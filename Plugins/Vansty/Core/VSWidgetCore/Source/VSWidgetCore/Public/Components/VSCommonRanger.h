// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "VSCommonRanger.generated.h"

class UCheckBox;
class UTextBlock;
class USpinBox;
class USlider;

/**
 * Widget that provides ranged value editing.
 *
 * Supports slider/spinbox inputs, optional navigation stepping, and
 * formatted content text output.
 */
UCLASS()
class VSWIDGETCORE_API UVSCommonRanger : public UCommonButtonBase
{
	GENERATED_UCLASS_BODY()

	DECLARE_MULTICAST_DELEGATE_TwoParams(FValueChangedSignature, UVSCommonRanger* /** Ranger */, float /** Value */);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FValueChangedEvent, UVSCommonRanger*, Ranger, float, Value);

public:
	//~ Begin UCommonButtonBase Interface
	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;
	virtual bool Initialize() override;
	virtual FNavigationReply NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply) override;
	//~ End UCommonButtonBase Interface
	
	/** Sets current value (clamped to ValueRange). */
	UFUNCTION(BlueprintCallable, Category = "Ranger")
	void SetValue(float NewValue);

	/** Returns current value. */
	UFUNCTION(BlueprintCallable, Category = "Ranger")
	virtual float GetValue() const { return CurrentValue; }
	
	/** Returns formatted content text for current value. */
	UFUNCTION(BlueprintCallable, Category = "Ranger")
	virtual FText GetContentText() const;

	/** Applies current settings to bound widget parts. */
	UFUNCTION(BlueprintCallable, Category = "Ranger")
	virtual void RefreshRanger();

protected:
	virtual void OnValueChangedInternal();
	void RefreshContentText();

	UFUNCTION()
	virtual void OnWidgetValueChanged(float Value);

private:
	void OnCultureChanged();
	
protected:
	/** Optional slider used for ranged input. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<USlider> Slider;

	/** Optional spin box used for ranged input. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<USpinBox> SpinBox;
	
	/** Optional display text showing the formatted current value. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TextBlock_Content;

public:
	/** Native event fired when value changes. */
	FValueChangedSignature OnValueChanged_Native;

	/** Blueprint event fired when value changes. */
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FValueChangedEvent OnValueChanged;
	
public:
	/** Min/max value range. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranger")
	FVector2D ValueRange = FVector2D(0.0, 1.0);

	/** Step size used by navigation/slider/spinbox. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranger")
	float StepSize = 0.f;
	
	/** Whether input should snap to StepSize. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranger")
	uint8 bSnapByStep : 1;

	/** If true, skips runtime auto-refresh in `NativePreConstruct` and expects manual `RefreshRanger`. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranger")
	uint8 bDifferRefreshment : 1;
	
	/** Value text format. Use `{0}` as placeholder for numeric value text. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranger")
	FText DisplayTextFormat = FText::FromString("{0}");
	
	/** Fractional digit range used when formatting value text. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranger")
	FIntPoint DisplayFractionDigitRange = FIntPoint(0, 324);

	/** Multiplier used for UI display and formatting. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranger")
	float DisplayValueMultiplier = 1.f;

private:
#if WITH_EDITORONLY_DATA
	/** Editor preview non-muted value. */
	UPROPERTY(EditAnywhere, DisplayName = "Preview Value", Category = "Ranger")
	float EditorPreviewValue = 0.f;
#endif

	/** Raw value synchronized with Slider and SpinBox. */
	float CurrentValue = 0.f;
	/** Tracks whether slider/spinbox delegates are currently bound. */
	uint8 bValueDelegatesBound : 1;
};
