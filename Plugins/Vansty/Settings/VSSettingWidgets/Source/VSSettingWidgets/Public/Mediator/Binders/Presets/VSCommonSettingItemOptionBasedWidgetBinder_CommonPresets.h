// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Mediator/Binders/VSCommonSettingItemOptionBasedWidgetBinder.h"
#include "VSCommonSettingItemOptionBasedWidgetBinder_CommonPresets.generated.h"

/**
 * Common preset option provider keyed by built-in setting item gameplay tags.
 */
UCLASS()
class VSSETTINGWIDGETS_API UVSCommonSettingItemOptionBasedWidgetBinder_CommonPresets : public UVSCommonSettingItemOptionBasedWidgetBinder
{
	GENERATED_UCLASS_BODY()

protected:
	//~ Begin UVSOptionBasedWidgetBinder Interface
	virtual TArray<FString> GenerateOptions_Implementation() const override;
	virtual TArray<FString> GenerateDisabledOptions_Implementation() const override;
	//~ End UVSOptionBasedWidgetBinder Interface
};
