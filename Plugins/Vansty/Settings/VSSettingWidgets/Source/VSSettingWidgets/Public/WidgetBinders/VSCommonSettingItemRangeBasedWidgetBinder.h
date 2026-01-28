// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBinders/VSRangeBasedWidgetBinder.h"
#include "WidgetControllers/VSSettingItemWidgetMediatorInterface.h"
#include "VSCommonSettingItemRangeBasedWidgetBinder.generated.h"

class UVSSettingItem;
class UVSCommonSettingItem;

/**
 * 
 */
UCLASS()
class VSSETTINGWIDGETS_API UVSCommonSettingItemRangeBasedWidgetBinder : public UVSRangeBasedWidgetBinder, public IVSSettingItemWidgetMediatorInterface
{
	GENERATED_UCLASS_BODY()

protected:
	//~ Begin UVSRangeBasedWidgetBinder Interface
	virtual void BindTypedWidget_Implementation(const FName TypeName, UWidget* Widget) override;
	virtual void UnbindTypedWidget_Implementation(const FName TypeName, UWidget* Widget) override;

	virtual void OnWidgetValueChanged_Implementation(float NewValue) override;
	virtual void OnCurrentSettingItemUpdated_Implementation() override;

#if WITH_EDITOR
	virtual void EditorRefreshMediator_Implementation() override;
#endif
	//~ End UVSRangeBasedWidgetBinder Interface
};
