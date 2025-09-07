// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSSettingItemBase.h"
#include "VSSettingItemBase_Float.generated.h"

enum class ERotatorDirection : uint8;

/**
 * 
 */
UCLASS(Abstract, DisplayName = "Settings.Item.Base.Float")
class VSSETTINGSYSTEM_API UVSSettingItemBase_Float : public UVSSettingItemBase
{
	GENERATED_UCLASS_BODY()

public:
	virtual void PostInitProperties() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	virtual void Initialize_Implementation() override;
	virtual void Validate_Implementation() override;
	virtual void SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) override;
	virtual bool EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const override;
	virtual void OnItemValueUpdated_Implementation() override;
	
	virtual void BindWidgetInternal_Implementation(UWidget* Widget, FName TypeName = NAME_None) override;
	virtual void UnbindWidgetInternal_Implementation(UWidget* Widget, FName TypeName) override;

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings")
	void SetValue(float InValue);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings")
	float GetValue(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::Settings) const;
	
protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings")
	FText GetValueContentText(float InValue) const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings")
	TArray<float> CalcGeneratedOptions() const;

	UFUNCTION(BlueprintCallable, Category = "Settings")
	TArray<float> GetOptions() const;

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void RefreshOptions();

	/** Widget to settings. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	float GetConditionToSettingValue(const float InValue);

	/** Settings to widget. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	float GetSettingToConditionValue(const float InValue);
	
private:
	UFUNCTION()
	void OnValueComboBoxStringSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnValueRotatorRotatedWithDirection(int32 NewValue, ERotatorDirection RotatorDir);

	UFUNCTION()
	void OnValueSliderValueChanged(float Value);
	
protected:
	/** This is mainly used in widgets and option generation. Will be used to clamp value. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	FVector2D DesiredValueRange = FVector2D(0.0, 1.0);

	/** This is mainly used in widgets and option generation. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	float DesiredValueStep = 0.01f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	bool bEnableLowerBoundExtensionReroute = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value", meta = (EditCondition = "bEnableLowerBoundExtensionReroute"))
	float LowerBoundExtensionRerouteValue = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	bool bEnableUpperBoundExtensionReroute = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value", meta = (EditCondition = "bEnableUpperBoundExtensionReroute"))
	float UpperBoundExtensionRerouteValue = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Options")
	bool bUseGeneratedOptions = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Options", meta = (EditCondition = "!bUseGeneratedOptions"))
	TArray<float> CustomOptions;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Options")
	bool bOptionsHighToLow = true;
	
	/** If not assigned, will use the value itself as name. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Texts")
	TMap<float, FText> NamedValues;

	/** Only works when no named value is matched. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Texts")
	FText ContentTextFormat = FText::FromString("{0}");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Texts")
	int32 DisplayFractionNum = 2;

private:
	UPROPERTY(VisibleAnywhere, Category = "Options", Transient)
	TArray<float> GeneratedOptions;
};
