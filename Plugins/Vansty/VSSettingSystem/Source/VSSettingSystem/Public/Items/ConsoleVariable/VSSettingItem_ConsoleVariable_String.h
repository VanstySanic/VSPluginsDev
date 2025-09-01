// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VSSettingItem_ConsoleVariableBase.h"
#include "Types/VSSettingSystemTypes.h"
#include "VSSettingItem_ConsoleVariable_String.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Settings.Item.ConsoleVariable.String")
class VSSETTINGSYSTEM_API UVSSettingItem_ConsoleVariable_String : public UVSSettingItem_ConsoleVariableBase
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
	UFUNCTION(BlueprintCallable, Category = "Settings", meta = (AutoCreateRefTerm = "InValue"))
	void SetValue(const FString& InValue);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	FString GetValue(EVSSettingItemValueSource::Type ValueType = EVSSettingItemValueSource::Settings) const;

private:
	FString Value;
	FString LastConfirmedValue;
};
