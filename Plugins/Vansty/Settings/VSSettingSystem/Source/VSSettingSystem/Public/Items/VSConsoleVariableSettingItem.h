// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSCommonSettingItem.h"
#include "VSConsoleVariableSettingItem.generated.h"

/** Defines how this item reacts when the bound console variable is modified externally. */
UENUM(BlueprintType)
namespace EVSSettingItemConsoleVariableChangePolicy
{
	enum Type
	{
		/** Do nothing. */
		None,
		
		/** Override the console variable with the applied item value. */
		Override,
		
		/** Absorb the setting item value form console variable. */
		Absorb
	};
}

/**
 * Setting item that binds to a console variable (CVar).
 *
 * Uses the configured ValueType to interpret the CVar value, and optionally keeps the item and
 * the CVar synchronized when changes occur from either side.
 */
UCLASS(DisplayName = "VS.Settings.Item.ConsoleVariable")
class VSSETTINGSYSTEM_API UVSConsoleVariableSettingItem : public UVSCommonSettingItem
{
	GENERATED_UCLASS_BODY()

public:
	//~ Begin UObject Interface
	virtual void PostLoad() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~ End UObject Interface

protected:
	//~ Begin UVSSettingItem Interface
	virtual void Initialize_Implementation() override;
	virtual void Uninitialize_Implementation() override;
	virtual void Apply_Implementation() override;
	
	virtual bool GetBooleanValue_Implementation(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const override;
	virtual int32 GetIntegerValue_Implementation(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const override;
	virtual int64 GetLongIntegerValue_Implementation(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const override;
	virtual float GetFloatValue_Implementation(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const override;
	virtual double GetDoubleValue_Implementation(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const override;
	virtual FString GetStringValue_Implementation(const EVSSettingItemValueSource::Type ValueSource = EVSSettingItemValueSource::System) const override;

#if WITH_EDITOR
	virtual bool AllowEditorChangingValueType_Implementation() const override;
#endif
	//~ End UVSSettingItem Interface

protected:
	IConsoleVariable* GetConsoleVariable() const { return CurrentConsoleVariable; }
	void SetConsoleVariableName(FString VariableName);

#if WITH_EDITOR
	/** Whether EditorPreviewValue can be edited in the editor for this item. */
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	bool AllowEditorChangingConsoleVariableName() const;
#endif

private:
	void OnConsoleVariableChanged(IConsoleVariable* Variable);
	
protected:
	/** Name of the console variable to bind to (e.g. "r.ShadowQuality"). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (EditCondition = "AllowEditorChangingConsoleVariableName()"))
	FString ConsoleVariableName;

	/** Defines what to do when the console variable changes outside the item. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TEnumAsByte<EVSSettingItemConsoleVariableChangePolicy::Type> ConsoleVariableChangePolicy = EVSSettingItemConsoleVariableChangePolicy::Override;
	
private:
	IConsoleVariable* CurrentConsoleVariable = nullptr;
	FDelegateHandle OnVariableChangedDelegateHandle;
};
