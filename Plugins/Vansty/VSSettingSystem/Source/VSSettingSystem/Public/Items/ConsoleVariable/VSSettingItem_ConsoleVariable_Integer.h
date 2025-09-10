// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSSettingItemBase_Integer.h"
#include "Types/VSSettingSystemTypes.h"
#include "VSSettingItem_ConsoleVariable_Integer.generated.h"

enum class ERotatorDirection : uint8;

/**
 * 
 */
UCLASS(DisplayName = "Settings.Item.ConsoleVariable.Integer")
class VSSETTINGSYSTEM_API UVSSettingItem_ConsoleVariable_Integer : public UVSSettingItemBase_Integer
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

	virtual int32 GetValue_Implementation(EVSSettingItemValueSource::Type ValueType = EVSSettingItemValueSource::Settings) const override;
	virtual void SetValue_Implementation(int32 InValue) override;

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
	int32 Value = 0;
	int32 LastConfirmedValue = 0;
};
