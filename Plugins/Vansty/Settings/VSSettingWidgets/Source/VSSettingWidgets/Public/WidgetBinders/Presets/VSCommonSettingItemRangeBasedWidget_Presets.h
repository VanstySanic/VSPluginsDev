// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBinders/VSRangeBasedWidgetBinder.h"
#include "VSCommonSettingItemRangeBasedWidget_Presets.generated.h"

/**
 * 
 */
UCLASS()
class VSSETTINGWIDGETS_API UVSCommonSettingItemRangeBasedWidget_Presets : public UVSRangeBasedWidgetBinder
{
	GENERATED_UCLASS_BODY()

protected:
	//~ Begin UVSRangeBasedWidgetBinder Interface
	virtual FVector2D GenerateValueRange_Implementation() const override;
	//~ End UVSRangeBasedWidgetBinder Interface
};
