// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSCommonSettingItem.h"
#include "VSConsoleVariableSettingItem.generated.h"

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
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	virtual void PostLoad() override;
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
	virtual bool EditorAllowChangingValueType_Implementation() const override;
	//~ End UVSSettingItem Interface
	
	/** Whether ConsoleVariableName can be edited in the editor for this item. */
	UFUNCTION(BlueprintNativeEvent, Category = "Settings")
	bool EditorAllowChangingConsoleVariableName() const;
#endif

	IConsoleVariable* GetConsoleVariable() const { return CurrentConsoleVariable; }
	/** Resolves and rebinds to a new console variable name. */
	void SetConsoleVariableName(FString VariableName);
	void OnConsoleVariableChanged(IConsoleVariable* Variable);
	
protected:
	/** Name of the console variable to bind to (e.g. "r.ShadowQuality"). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (EditCondition = "EditorAllowChangingConsoleVariableName()"))
	FString ConsoleVariableName;
	
private:
	IConsoleVariable* CurrentConsoleVariable = nullptr;
};
