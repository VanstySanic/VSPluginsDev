// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSSettingItemBase.h"
#include "VSSettingItemBase_String.generated.h"

enum class ERotatorDirection : uint8;

/**
 * 
 */
UCLASS(Abstract, DisplayName = "Settings.Item.Base.String")
class VSSETTINGSYSTEM_API UVSSettingItemBase_String : public UVSSettingItemBase
{
	GENERATED_UCLASS_BODY()

public:
	virtual void PostInitProperties() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
protected:
	virtual void Initialize_Implementation() override;
	virtual void SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) override;
	virtual bool EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const override;
	virtual void OnItemValueUpdated_Implementation() override;
	
	virtual void BindWidgetInternal_Implementation(UWidget* Widget, FName TypeName = NAME_None) override;
	virtual void UnbindWidgetInternal_Implementation(UWidget* Widget, FName TypeName) override;

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings", meta = (AutoCreateRefTerm = "InValue"))
	void SetValue(const FString& InValue);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings")
	FString GetValue(EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::Settings) const;

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Settings", meta = (AutoCreateRefTerm = "InValue"))
	FText GetValueContentText(const FString& InValue) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Settings")
	TArray<FString> CalcGeneratedOptions() const;

	UFUNCTION(BlueprintCallable, Category = "Settings")
	TArray<FString> GetOptions() const;

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void RefreshOptions();
	
private:
	UFUNCTION()
	void OnValueComboBoxStringSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnValueRotatorRotatedWithDirection(int32 NewValue, ERotatorDirection RotatorDir);

	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Options")
	bool bUseGeneratedOptions = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Options", meta = (EditCondition = "!bUseGeneratedOptions"))
	TArray<FString> CustomOptions;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Options")
	bool bSortOptions = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Options", meta = (EditCondition = "bSortOptions"))
	bool bSortOptionsByName = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Options", meta = (EditCondition = "bSortOptions"))
	bool bSortOptionsHighToLow = false;

	/** If not assigned, will use the value itself as name. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Texts")
	TMap<FString, FText> NamedValues;

private:
	UPROPERTY(VisibleAnywhere, Category = "Options", Transient)
	TArray<FString> GeneratedOptions;
};
