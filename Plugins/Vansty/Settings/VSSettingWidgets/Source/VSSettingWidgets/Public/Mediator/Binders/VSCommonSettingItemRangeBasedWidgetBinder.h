// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Mediator/Binders//VSRangeBasedWidgetBinder.h"
#include "Mediator/VSSettingItemWidgetMediatorInterface.h"
#include "VSCommonSettingItemRangeBasedWidgetBinder.generated.h"

class UVSSettingItemBase;
class UVSCommonSettingItem;

/**
 * Range binder adapter for float-based common setting items.
 *
 * It routes the generic "Core" widget to the range channel and synchronizes float value changes
 * between UI widgets and the resolved setting item.
 */
UCLASS()
class VSSETTINGWIDGETS_API UVSCommonSettingItemRangeBasedWidgetBinder : public UVSRangeBasedWidgetBinder, public IVSSettingItemWidgetMediatorInterface
{
	GENERATED_UCLASS_BODY()

protected:
	//~ Begin UVSRangeBasedWidgetBinder Interface
	virtual void BindTypedWidget_Implementation(const FName TypeName, UWidget* Widget) override;
	virtual void UnbindTypedWidget_Implementation(const FName TypeName, UWidget* Widget) override;
	virtual void OnCurrentSettingItemUpdated_Implementation() override;

	virtual float GetExternalValue_Implementation() const override;
	virtual void OnWidgetValueChanged_Implementation(float NewValue) override;

#if WITH_EDITOR
	virtual void EditorRefreshMediator_Implementation() override;
#endif
	//~ End UVSRangeBasedWidgetBinder Interface
};
