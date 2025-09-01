// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSSettingItem_ConsoleVariableBase.h"
#include "Types/VSSettingSystemTypes.h"
#include "VSSettingItem_ConsoleVariable_Float.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Settings.Item.ConsoleVariable.Float")
class VSSETTINGSYSTEM_API UVSSettingItem_ConsoleVariable_Float : public UVSSettingItem_ConsoleVariableBase
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void Load_Implementation() override;
	virtual void Apply_Implementation() override;
	virtual void Confirm_Implementation() override;
	virtual void Save_Implementation() override;
	virtual void SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) override;
	virtual bool EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const override;
	
	virtual void OnConsoleVariableChanged(IConsoleVariable* Variable) override;
	
public:
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetValue(float InValue);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	float GetValue(EVSSettingItemValueSource::Type ValueType = EVSSettingItemValueSource::Settings) const;
	
public:


private:
	float Value = 0.f;
	float LastConfirmedValue = 0.f;
};
