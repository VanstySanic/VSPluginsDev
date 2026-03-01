// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSWidgetBinder.h"
#include "VSRangeBasedWidgetBinder.generated.h"

class UVSCommonRanger;
class UVSCommonMutableRanger;

/**
 * Binder for range/value widgets.
 *
 * Synchronizes external value state with widgets bound as `Range` and
 * optional display widgets bound as `Content`.
 */
UCLASS(DisplayName = "VS.Widget.Binder.RangeBased")
class VSWIDGETCORE_API UVSRangeBasedWidgetBinder : public UVSWidgetBinder
{
	GENERATED_UCLASS_BODY()

protected:
	//~ Begin UVSWidgetBinder Interface
	virtual void Initialize_Implementation() override;
	virtual void Uninitialize_Implementation() override;
	virtual void BindTypedWidget_Implementation(const FName TypeName, UWidget* Widget) override;
	virtual void UnbindTypedWidget_Implementation(const FName TypeName, UWidget* Widget) override;
	//~ End UVSWidgetBinder Interface

public:
	/** Returns external value from game data source. */
	UFUNCTION(BlueprintNativeEvent, Category = "Range")
	float GetExternalValue() const;
	
	/** Returns value currently represented by bound range widget. */
	UFUNCTION(BlueprintCallable, Category = "Range")
	float GetWidgetValue() const;

	UFUNCTION(BlueprintCallable, Category = "Range")
	FVector2D GetValueRange() const { return ValueRange; }

	/** Returns formatted content text based on current widget value. */
	UFUNCTION(BlueprintCallable, Category = "Range")
	FText GetContentText() const;
	
	/** Regenerates range data and rebinds related widgets. */
	UFUNCTION(BlueprintCallable, Category = "Range")
	void RefreshRange();

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Range")
	FVector2D GenerateValueRange() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Range")
	void OnWidgetValueChanged(float NewValue);

#if WITH_EDITOR
	UFUNCTION(BlueprintNativeEvent, Category = "Range")
	bool EditorAllowChangingStepSize() const;
	
	UFUNCTION(BlueprintNativeEvent, Category = "Range")
	bool EditorAllowChangingSnapByStep() const;
#endif

private:
	void OnCultureChanged();
	void OnCommonRangerValueChanged(UVSCommonRanger* Ranger, float Value);

	UFUNCTION()
	void OnDisplayWidgetValueChanged(float Value);
	
	UFUNCTION()
	void OnBoundWidgetValueChanged(float Value);

protected:
	UPROPERTY(VisibleAnywhere, Category = "Range")
	FVector2D ValueRange = FVector2D(0.0, 1.0);

	UPROPERTY(EditAnywhere, Category = "Range", meta = (EditCondition = "EditorAllowChangingStepSize()"))
	float StepSize = 0.f;
	
	UPROPERTY(EditAnywhere, Category = "Range", meta = (EditCondition = "EditorAllowChangingSnapByStep()"))
	uint8 bSnapByStep : 1;

	
	/** Value text format. Use `{0}` as placeholder for numeric value text. */
	UPROPERTY(EditAnywhere, Category = "Range")
	FText DisplayTextFormat = FText::FromString("{0}");
	
	/** Fractional digit range used when formatting value text. */
	UPROPERTY(EditAnywhere, Category = "Range")
	FIntPoint DisplayFractionDigitRange = FIntPoint(0, 324);
	
	/** Multiplier used for UI display and formatting. */
	UPROPERTY(EditAnywhere, Category = "Range")
	float DisplayValueMultiplier = 1.f;
};
