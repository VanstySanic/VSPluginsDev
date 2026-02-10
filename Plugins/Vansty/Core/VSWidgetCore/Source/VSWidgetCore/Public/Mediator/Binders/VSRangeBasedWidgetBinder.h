// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSWidgetBinder.h"
#include "VSRangeBasedWidgetBinder.generated.h"

class UVSCommonRanger;
class UVSCommonMutableRanger;

/**
 * 
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
	/** Get the real value in game, not in the bound widget. */
	UFUNCTION(BlueprintNativeEvent, Category = "Range")
	float GetExternalValue() const;
	
	/** Get the value from widget. */
	UFUNCTION(BlueprintCallable, Category = "Range")
	float GetCurrentValue() const;

	UFUNCTION(BlueprintCallable, Category = "Range")
	FVector2D GetValueRange() const { return ValueRange; }

	UFUNCTION(BlueprintCallable, Category = "Range")
	FText GetContentText() const;
	
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

	
	/** The content text format to display. If you want to show the digits, please put ‘{0}’ in it. */
	UPROPERTY(EditAnywhere, Category = "Range")
	FText DisplayTextFormat = FText::FromString("{0}");
	
	UPROPERTY(EditAnywhere, Category = "Range")
	FIntPoint DisplayFractionDigitRange = FIntPoint(0, 324);
	
	UPROPERTY(EditAnywhere, Category = "Range")
	float DisplayValueMultiplier = 1.f;
};
