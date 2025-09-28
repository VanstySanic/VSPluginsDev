// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VSSettingWidgetInterface.generated.h"

struct FGameplayTag;

// This class does not need to be modified.
UINTERFACE()
class VSSETTINGSYSTEM_API UVSSettingWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VSSETTINGSYSTEM_API IVSSettingWidgetInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Widget", meta = (AutoCreateRefTerm = "SpecifyTag"))
	void BindWidgetToSettingItem(const FGameplayTag& SpecifyTag);

	UFUNCTION(BlueprintNativeEvent, Category = "Widget", meta = (AutoCreateRefTerm = "SpecifyTag"))
	void UnbindWidgetFromSettingItem(const FGameplayTag& SpecifyTag);
};