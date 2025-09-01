// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/VSSettingItemBase.h"
#include "UObject/Object.h"
#include "VSSettingItem_ConsoleVariableBase.generated.h"

/**
 * 
 */
UCLASS(Abstract, DisplayName = "Settings.Item.ConsoleVariable.Base")
class VSSETTINGSYSTEM_API UVSSettingItem_ConsoleVariableBase : public UVSSettingItemBase
{
	GENERATED_UCLASS_BODY()

protected:
	virtual void Initialize_Implementation() override;
	virtual void Uninitialize_Implementation() override;

public:
	IConsoleVariable* GetConsoleVariable() const { return ConsoleVariable; }

protected:
	virtual void OnConsoleVariableChanged(IConsoleVariable* Variable);

protected:
	UPROPERTY(EditAnywhere, Category = "Settings")
	FString ConsoleVariableName;

	UPROPERTY(EditAnywhere, Category = "Settings")
	bool bListenAndOverride = true;

private:
	IConsoleVariable* ConsoleVariable = nullptr;
	FDelegateHandle OnVariableChangedDelegateHandle;
};
