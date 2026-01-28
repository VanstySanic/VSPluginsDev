// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBinders/VSCommonSettingItemOptionBasedWidgetBinder.h"
#include "VSCommonSettingItemOptionBasedWidgetBinder_Presets.generated.h"

/**
 * 
 */
UCLASS()
class VSSETTINGWIDGETS_API UVSCommonSettingItemOptionBasedWidgetBinder_Presets : public UVSCommonSettingItemOptionBasedWidgetBinder
{
	GENERATED_UCLASS_BODY()

protected:
	//~ Begin UVSOptionBasedWidgetBinder Interface
	virtual TArray<FString> GenerateOptions_Implementation() const override;
	virtual TArray<FString> GenerateDisabledOptions_Implementation() const override;
	//~ End UVSOptionBasedWidgetBinder Interface
	
public:

};
