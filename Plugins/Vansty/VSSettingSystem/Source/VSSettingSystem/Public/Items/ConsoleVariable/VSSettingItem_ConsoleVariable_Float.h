// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSSettingItemBase_Float.h"
#include "Types/VSSettingSystemTypes.h"
#include "VSSettingItem_ConsoleVariable_Float.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Settings.Item.ConsoleVariable.Float")
class VSSETTINGSYSTEM_API UVSSettingItem_ConsoleVariable_Float : public UVSSettingItemBase_Float
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

	virtual float GetValue_Implementation(EVSSettingItemValueSource::Type ValueType = EVSSettingItemValueSource::Settings) const override;
	virtual void SetValue_Implementation(float InValue) override;

public:
	IConsoleVariable* GetConsoleVariable() const { return ConsoleVariable; }
	
private:
	void OnConsoleVariableChanged(IConsoleVariable* Variable);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	FString ConsoleVariableName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	bool bListenAndOverride = true;
	
private:
	IConsoleVariable* ConsoleVariable = nullptr;
	FDelegateHandle OnVariableChangedDelegateHandle;
	float Value = 0.f;
	float LastConfirmedValue = 0.f;
};
