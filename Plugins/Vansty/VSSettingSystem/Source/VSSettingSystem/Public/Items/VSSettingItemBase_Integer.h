// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSSettingItemBase.h"
#include "VSSettingItemBase_Integer.generated.h"

enum class ERotatorDirection : uint8;

/**
 * 
 */
UCLASS(Abstract, DisplayName = "Settings.Item.Base.Integer")
class VSSETTINGSYSTEM_API UVSSettingItemBase_Integer : public UVSSettingItemBase
{
	GENERATED_UCLASS_BODY()

public:
	virtual void PostInitProperties() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
protected:
	virtual void Initialize_Implementation() override;
	
protected:
	virtual void Validate_Implementation() override;
	virtual void SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) override;
	virtual bool EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const override;
	virtual void OnItemValueUpdated_Implementation() override;
	
	virtual void BindWidgetInternal_Implementation(UWidget* Widget, FName TypeName = NAME_None) override;
	virtual void UnbindWidgetInternal_Implementation(UWidget* Widget, FName TypeName) override;

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings")
	void SetValue(int32 InValue);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings")
	int32 GetValue(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::Settings) const;

protected:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	FText GetValueContentText(int32 InValue) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings")
	TArray<int32> CalcGeneratedOptions() const;

	UFUNCTION(BlueprintCallable, Category = "Settings")
	TArray<int32> GetOptions() const;

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void RefreshOptions();


	/** Widget to settings. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	int32 GetConditionToSettingValue(const int32 InValue);

	/** Settings to widget. */
	UFUNCTION(BlueprintCallable, Category = "Settings")
	int32 GetSettingToConditionValue(const int32 InValue);
	
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
	FIntPoint DesiredValueRange = FIntPoint(0, 1);

	/** This is mainly used in widgets and option generation. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	int32 DesiredValueStep = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	bool bEnableLowerBoundExtensionReroute = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value", meta = (EditCondition = "bEnableLowerBoundExtensionReroute"))
	int32 LowerBoundExtensionRerouteValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
	bool bEnableUpperBoundExtensionReroute = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value", meta = (EditCondition = "bEnableUpperBoundExtensionReroute"))
	int32 UpperBoundExtensionRerouteValue = 0;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Options")
	bool bUseGeneratedOptions = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Options", meta = (EditCondition = "!bUseGeneratedOptions"))
	TArray<int32> CustomOptions;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Options")
	bool bOptionsHighToLow = true;

	
	/** If not assigned, will use the value itself as name. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Texts")
	TMap<int32, FText> NamedValues;
	
	/** Only works when no named value is matched. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Texts")
	FText ContentTextFormat = FText::FromString("{0}");

private:
	UPROPERTY(VisibleAnywhere, Category = "Options", Transient)
	TArray<int32> GeneratedOptions;
};
