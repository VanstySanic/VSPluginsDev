// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBinders/VSCommonSettingItemOptionBasedWidgetBinder.h"
#include "VSCommonSettingItemOptionBasedWidgetBinder_ScalabilityLevel.generated.h"

/**
 * 
 */
UCLASS()
class VSSETTINGWIDGETS_API UVSCommonSettingItemOptionBasedWidgetBinder_ScalabilityLevel : public UVSCommonSettingItemOptionBasedWidgetBinder
{
	GENERATED_UCLASS_BODY()

protected:
	//~ Begin UVSOptionBasedWidgetBinder Interface
	virtual TMap<FString, FText> GetOptionKeyedTexts_Implementation() const override;
	//~ End UVSOptionBasedWidgetBinder Interface

public:


protected:
	
};
