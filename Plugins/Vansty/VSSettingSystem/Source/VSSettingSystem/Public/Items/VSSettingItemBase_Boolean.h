// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSSettingItemBase.h"
#include "VSSettingItemBase_Boolean.generated.h"

/**
 * 
 */
UCLASS(Abstract, DisplayName = "Settings.Item.Base.Boolean")
class VSSETTINGSYSTEM_API UVSSettingItemBase_Boolean : public UVSSettingItemBase
{
	GENERATED_UCLASS_BODY()

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	virtual void SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) override;
	virtual bool EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const override;
	virtual void OnItemValueUpdated_Implementation() override;
	
	virtual void BindWidgetInternal_Implementation(UWidget* Widget, FName TypeName = NAME_None) override;
	virtual void UnbindWidgetInternal_Implementation(UWidget* Widget, FName TypeName) override;

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings")
	void SetValue(bool bInValue);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings")
	bool GetValue(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::Settings) const;

protected:
	UFUNCTION()
	void OnValueComboBoxStringSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnValueCheckBoxCheckStateChanged(bool bIsChecked);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", EditFixedSize = "2", meta = (EditFixedOrder))
	TMap<bool, FText> NamedBooleans;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	bool bOptionsTrueThenFalse = true;
};
