// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBinders/VSSettingItemMutableRangedWidgetBinder.h"
#include "VSSettingItemMutableRangedWidgetBinder_CommonPresets.generated.h"

/**
 * 
 */
UCLASS()
class VSSETTINGWIDGETS_API UVSSettingItemMutableRangedWidgetBinder_CommonPresets : public UVSSettingItemMutableRangedWidgetBinder
{
	GENERATED_UCLASS_BODY()

	//~ Begin UVSSettingItemMutableRangedWidgetBinder Interface
	virtual FVector2D GenerateValueRange_Implementation() const override;

#if WITH_EDITOR
	virtual bool EditorAllowChangingStepSize_Implementation() const override;
	virtual bool EditorAllowChangingSnapByStep_Implementation() const override;
	
	virtual void EditorRefreshMediator_Implementation() override;
#endif
	//~ End UVSSettingItemMutableRangedWidgetBinder Interface
};
