// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSSettingItemBase_Boolean.h"
#include "Types/VSSettingSystemTypes.h"
#include "VSSettingItem_ConsoleVariable_Boolean.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Settings.Item.ConsoleVariable.Boolean")
class VSSETTINGSYSTEM_API UVSSettingItem_ConsoleVariable_Boolean : public UVSSettingItemBase_Boolean
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void Initialize_Implementation() override;
	virtual void Uninitialize_Implementation() override;

	virtual void Load_Implementation() override;
	virtual void Apply_Implementation() override;
	virtual void Confirm_Implementation() override;
	virtual void Save_Implementation() override;
	virtual void SetToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) override;
	virtual bool EqualsToBySource_Implementation(const EVSSettingItemValueSource::Type ValueSource) const override;
	virtual void OnItemValueUpdated_Implementation() override;

	virtual void SetValue_Implementation(bool bInValue) override;
	virtual bool GetValue_Implementation(EVSSettingItemValueSource::Type ValueType = EVSSettingItemValueSource::Settings) const override;
	
public:
	IConsoleVariable* GetConsoleVariable() const { return ConsoleVariable; }
	
private:
	void OnConsoleVariableChanged(IConsoleVariable* Variable);

protected:
	UPROPERTY(EditAnywhere, Category = "Settings")
	FString ConsoleVariableName;

	UPROPERTY(EditAnywhere, Category = "Settings")
	bool bListenAndOverride = true;

private:
	IConsoleVariable* ConsoleVariable = nullptr;
	FDelegateHandle OnVariableChangedDelegateHandle;
	bool bValue = false;
	bool bLastConfirmedValue = false;
};
