// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSWidgetBinder.h"
#include "VSRangeBasedWidgetBinder.generated.h"

class UVSCommonRanger;

/**
 * 
 */
UCLASS()
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
	/** Get the real value in game, not in the bound widget. Need to be overriden. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Range")
	float GetExternalValue(bool bIgnoreMutedState) const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Range")
	bool GetExternalIsMuted() const;
	
	/** Get the widget value. */
	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Range")
	float GetCurrentValue(bool bIgnoreMutedState = false) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Range")
	bool GetCurrentIsMuted() const;
	
	UFUNCTION(BlueprintCallable, Category = "Range")
	FVector2D GetValueRange() const { return ValueRange; }

	UFUNCTION(BlueprintCallable, Category = "Range")
	FText GetContentText(float Value, bool bMuted = false, bool bSameValueMutedText = true) const;
	
	UFUNCTION(BlueprintCallable, Category = "Range")
	void RefreshRange();

protected:
	/** Get the widget value that ignores the muted state. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Range")
	float GetCurrentNonMutedValue() const;
	
	UFUNCTION(BlueprintNativeEvent, Category = "Range")
	FVector2D GenerateValueRange() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Range")
	void OnWidgetValueChanged(float NewValue);

#if WITH_EDITOR
	UFUNCTION(BlueprintNativeEvent, Category = "Range")
	bool EditorAllowChangingStepSize() const;
	
	UFUNCTION(BlueprintNativeEvent, Category = "Range")
	bool EditorAllowChangingSnapByStep() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Range")
	bool EditorAllowChangingSupportMutation() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Range")
	bool EditorAllowChangingMutedStateValue() const;
#endif


private:
	void OnCultureChanged();
	void OnCommonRangerValueChanged(UVSCommonRanger* Ranger, float Value, bool bIsMuteRedirect);
	
	UFUNCTION()
	void OnBoundWidgetValueChanged(float Value);

	UFUNCTION()
	void OnWidgetMutedStateChanged(bool bIsMuted);

private:
	UPROPERTY(VisibleAnywhere, Category = "Range")
	FVector2D ValueRange = FVector2D(0.0, 1.0);

	UPROPERTY(EditAnywhere, Category = "Range", meta = (EditCondition = "EditorAllowChangingStepSize()"))
	float StepSize = 0.f;
	
	UPROPERTY(EditAnywhere, Category = "Range", meta = (EditCondition = "EditorAllowChangingSnapByStep()"))
	uint8 bSnapByStep : 1;

	UPROPERTY(EditAnywhere, Category = "Range", meta = (EditCondition = "EditorAllowChangingSupportMutation()"))
	uint8 bSupportMutation : 1;
	
	UPROPERTY(EditAnywhere, Category = "Range", meta = (EditCondition = "EditorAllowChangingMutedStateValue()"))
	float MutedStateValue = 0.f;
	
	
	/** The content text format to display. If you want to show the digits, please put ‘{0}’ in it. */
	UPROPERTY(EditAnywhere, Category = "Range")
	FText DisplayFormatText = FText::FromString("{0}");
	
	UPROPERTY(EditAnywhere, Category = "Range")
	FIntPoint DisplayFractionDigitRange = FIntPoint(1, 324);

	/** The text to display when muted. Not work if left empty. */
	UPROPERTY(EditAnywhere, Category = "Range")
	FText DisplayMutedText = FText::FromString("");
	
	UPROPERTY(EditAnywhere, Category = "Range")
	uint8 bDisplaySameValueMutedText : 1;
};
