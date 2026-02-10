// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Mediator/Binders/VSCommonSettingItemRangeBasedWidgetBinder.h"
#include "VSCommonSettingItemRangeBasedWidgetBinder_CommonPresets.generated.h"

/**
 * 
 */
UCLASS()
class VSSETTINGWIDGETS_API UVSCommonSettingItemRangeBasedWidgetBinder_CommonPresets : public UVSCommonSettingItemRangeBasedWidgetBinder
{
	GENERATED_UCLASS_BODY()

protected:
	//~ Begin UVSRangeBasedWidgetBinder Interface
	virtual FVector2D GenerateValueRange_Implementation() const override;

#if WITH_EDITOR
	virtual bool EditorAllowChangingStepSize_Implementation() const override;
	virtual bool EditorAllowChangingSnapByStep_Implementation() const override;
	
	virtual void EditorRefreshMediator_Implementation() override;
#endif
	//~ End UVSRangeBasedWidgetBinder Interface
	
};
