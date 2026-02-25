// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSWidgetBinder.h"
#include "VSMutableRangedWidgetBinder.generated.h"

class UVSCommonRanger;
class UVSCommonMutableRanger;

/**
 * 
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
	/** Get the real value in game, not in the bound widget. */
	UFUNCTION(BlueprintCallable, Category = "Range")
	float GetExternalValue() const;
	
	/** Get the external value that ignores the muted state. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Range")
	float GetExternalNonMutedValue() const;
	
	/** Get the real mute state in game, not in the bound widget. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Range")
	bool GetExternalIsMuted() const;

	/** Get the widget value that ignores the muted state. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Range")
	float GetWidgetNonMutedValue() const;
	
	/** Get the widget value. */
	UFUNCTION(BlueprintCallable, Category = "Range")
	float GetWidgetValue() const;
	
	/** Get the widget mute state. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Range")
	bool GetWidgetIsMuted() const;
	
	UFUNCTION(BlueprintCallable, Category = "Range")
	FVector2D GetValueRange() const { return ValueRange; }

	UFUNCTION(BlueprintCallable, Category = "Range")
	FText GetContentText() const;
	
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

	
	/** The content text format to display. If you want to show the digits, please put ‘{0}’ in it. */
	UPROPERTY(EditAnywhere, Category = "Range")
	FText DisplayTextFormat = FText::FromString("{0}");
	
	UPROPERTY(EditAnywhere, Category = "Range")
	FIntPoint DisplayFractionDigitRange = FIntPoint(1, 324);

	UPROPERTY(EditAnywhere, Category = "Range")
	float DisplayValueMultiplier = 1.f;
	
	/** The text to display when muted. Not work if left empty. */
	UPROPERTY(EditAnywhere, Category = "Range")
	FText DisplayMutedText = FText::FromString("");
};
