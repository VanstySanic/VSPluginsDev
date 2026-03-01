// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSWidgetBinder.h"
#include "VSMutableRangedWidgetBinder.generated.h"

class UVSCommonRanger;
class UVSCommonMutableRanger;

/**
 * Binder for ranged widgets with additional mute state support.
 *
 * Synchronizes `Range`, optional `Mute`, and optional `Content` typed
 * widgets with external non-muted value and mute-state data.
 */
UCLASS(DisplayName = "VS.Widget.Binder.MutableRanged")
class VSWIDGETCORE_API UVSMutableRangedWidgetBinder : public UVSWidgetBinder
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
	/** Returns external value (resolved with mute state). */
	UFUNCTION(BlueprintCallable, Category = "Range")
	float GetExternalValue() const;
	
	/** Get the external value that ignores the muted state. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Range")
	float GetExternalNonMutedValue() const;
	
	/** Returns external mute state. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Range")
	bool GetExternalIsMuted() const;

	/** Returns widget value without mute-state remapping. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Range")
	float GetWidgetNonMutedValue() const;
	
	/** Returns widget value (resolved with mute state). */
	UFUNCTION(BlueprintCallable, Category = "Range")
	float GetWidgetValue() const;
	
	/** Returns widget mute state. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Range")
	bool GetWidgetIsMuted() const;
	
	UFUNCTION(BlueprintCallable, Category = "Range")
	FVector2D GetValueRange() const { return ValueRange; }

	/** Returns formatted content text from current widget state. */
	UFUNCTION(BlueprintCallable, Category = "Range")
	FText GetContentText() const;
	
	/** Regenerates range/mute data and rebinds related widgets. */
	UFUNCTION(BlueprintCallable, Category = "Range")
	void RefreshRange();

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Range")
	FVector2D GenerateValueRange() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Range")
	float GenerateMuteStateValue() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Range")
	void OnWidgetValueChanged(float NewValue);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Range")
	void OnWidgetMuteStateChanged(bool bMuted);
	
#if WITH_EDITOR
	UFUNCTION(BlueprintNativeEvent, Category = "Range")
	bool EditorAllowChangingStepSize() const;
	
	UFUNCTION(BlueprintNativeEvent, Category = "Range")
	bool EditorAllowChangingSnapByStep() const;
#endif

private:
	void OnCultureChanged();
	void OnCommonRangerValueChanged(UVSCommonRanger* Ranger, float Value);
	void OnCommonRangerMuteStateChanged(UVSCommonMutableRanger* Ranger, bool bMuted);

	UFUNCTION()
	void OnDisplayWidgetValueChanged(float Value);
	
	UFUNCTION()
	void OnBoundWidgetValueChanged(float Value);

	UFUNCTION()
	void OnBoundWidgetMuteStateChanged(bool bMuted);

protected:
	UPROPERTY(VisibleAnywhere, Category = "Range")
	FVector2D ValueRange = FVector2D(0.0, 1.0);
	
	UPROPERTY(VisibleAnywhere, Category = "Range")
	float MuteStateValue = 0.f;
	
	UPROPERTY(EditAnywhere, Category = "Range", meta = (EditCondition = "EditorAllowChangingStepSize()"))
	float StepSize = 0.f;
	
	UPROPERTY(EditAnywhere, Category = "Range", meta = (EditCondition = "EditorAllowChangingSnapByStep()"))
	uint8 bSnapByStep : 1;

	
	/** Value text format. Use `{0}` as placeholder for numeric value text. */
	UPROPERTY(EditAnywhere, Category = "Range")
	FText DisplayTextFormat = FText::FromString("{0}");
	
	UPROPERTY(EditAnywhere, Category = "Range")
	FIntPoint DisplayFractionDigitRange = FIntPoint(1, 324);

	/** Multiplier used for UI display and formatting. */
	UPROPERTY(EditAnywhere, Category = "Range")
	float DisplayValueMultiplier = 1.f;
	
	/** Text used when muted. Ignored when empty. */
	UPROPERTY(EditAnywhere, Category = "Range")
	FText DisplayMutedText = FText::FromString("");
};
