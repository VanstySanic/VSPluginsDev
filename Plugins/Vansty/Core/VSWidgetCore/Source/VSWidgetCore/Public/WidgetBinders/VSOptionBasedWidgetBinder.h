// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSWidgetBinder.h"
#include "VSOptionBasedWidgetBinder.generated.h"

class UVSCommonRanger;
class UCommonButtonBase;

/**
 * Option-driven widget binder that synchronizes a logical option list with
 * a bound UI selector (e.g. ComboBox, CommonRotator, or button group).
 *
 * This binder builds a displayable option array and its localized texts,
 * applies selection to the bound widget based on the current game option,
 * and reports UI-driven index changes back to derived implementations.
 *
 * It also supports optional option reversal, disabled-option filtering, and
 * culture-change refresh for localized option text.
 */
UCLASS()
class VSWIDGETCORE_API UVSOptionBasedWidgetBinder : public UVSWidgetBinder
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
	/** Get the real option in game, not in the bound widget. Need to be overriden. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Option")
	FString GetExternalOption() const;

	/** Get the widget option index. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Option")
	int32 GetCurrentIndex() const;

	/** Get the widget option. */
	UFUNCTION(BlueprintCallable, Category = "Option")
	FString GetCurrentOption() const;
	
	UFUNCTION(BlueprintCallable, Category = "Option")
	FText GetCurrentOptionText() const;

	UFUNCTION(BlueprintCallable, Category = "Option", meta = (AutoCreateRefTerm = "Option"))
	int32 GetOptionIndex(const FString& Option) const;

	UFUNCTION(BlueprintCallable, Category = "Option")
	FString GetOptionAtIndex(int32 Index) const;

	UFUNCTION(BlueprintCallable, Category = "Option")
	FText GetOptionTextAtIndex(int32 Index) const;
	
	UFUNCTION(BlueprintCallable, Category = "Option", meta = (AutoCreateRefTerm = "Option"))
	FText GetTextByOption(const FString& Option) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Option", meta = (AutoCreateRefTerm = "Option"))
	FText OptionStringToText(const FString& Option) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Option", meta = (AutoCreateRefTerm = "Option"))
	bool IsOptionDisabled(const FString& Option);

	/** Refresh options and texts, and rebind the widget. */
	UFUNCTION(BlueprintCallable, Category = "Option")
	void RefreshOptions();
	
protected:
	/** Generates the full option list used by this binder. */
	UFUNCTION(BlueprintNativeEvent, Category = "Option")
	TArray<FString> GenerateOptions() const;

	/** Generate the options that can be displayed but should be disabled from widget selection. */
	UFUNCTION(BlueprintNativeEvent, Category = "Option")
	TArray<FString> GenerateDisabledOptions() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Option")
	void OnWidgetOptionChanged(int32 NewIndex);

private:
	void OnCultureChanged();
	void OnBoundWidgetValueChanged(int32 Index);

	UFUNCTION()
	void OnComboBoxStringOpening();

	UFUNCTION()
	void OnComboBoxStringValueChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnCommonRotatorValueChanged(int32 Value, ERotatorDirection RotatorDir);

	UFUNCTION()
	void OnButtonGroupValueChanged(UCommonButtonBase* AssociatedButton, int32 ButtonIndex);

	UFUNCTION()
	void OnSliderValueChanged(float Value);

	UFUNCTION()
	void OnCommonRangerValueChanged(UVSCommonRanger* Ranger, float Value);
	
protected:
	/** If true, reverse the generated options after generation. */
	UPROPERTY(EditAnywhere, Category = "Option")
	uint8 bInternalReverseOptions : 1;
	
	/**
	 * If true, disabled options will be removed from the option array.
	 * Otherwise, option item widgets of these options should be disabled.
	 */
	UPROPERTY(EditAnywhere, Category = "Option")
	uint8 bInternalHideDisabledOptions : 1;

private:
	UPROPERTY(VisibleAnywhere, Category = "Option")
	TArray<FString> Options;

	UPROPERTY(VisibleAnywhere, Category = "Option")
	TArray<FString> DisabledOptions;
	
	UPROPERTY(VisibleAnywhere, Category = "Option")
	TArray<FText> OptionTexts;
};
