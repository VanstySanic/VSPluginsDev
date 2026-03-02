// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Mediator/Binders/VSOptionBasedWidgetBinder.h"
#include "Mediator/VSSettingItemWidgetMediatorInterface.h"
#include "VSCommonSettingItemOptionBasedWidgetBinder.generated.h"

class UVSSettingItemBase;
class UVSCommonSettingItem;

/**
 * Option binder adapter for UVSCommonSettingItem string options.
 *
 * It maps the generic "Core" widget to the option channel and syncs selected option text/value
 * with the resolved setting item.
 */
UCLASS()
class VSSETTINGWIDGETS_API UVSCommonSettingItemOptionBasedWidgetBinder : public UVSOptionBasedWidgetBinder, public IVSSettingItemWidgetMediatorInterface
{
	GENERATED_UCLASS_BODY()

protected:
	//~ Begin UVSOptionBasedWidgetBinder Interface
	virtual void BindTypedWidget_Implementation(const FName TypeName, UWidget* Widget) override;
	virtual void UnbindTypedWidget_Implementation(const FName TypeName, UWidget* Widget) override;
	virtual void OnCurrentSettingItemUpdated_Implementation() override;

	virtual FString GetExternalOption_Implementation() const override;
	virtual FText OptionStringToText_Implementation(const FString& String) const override;
	virtual void OnWidgetOptionChanged_Implementation(int32 NewIndex) override;
	//~ End UVSOptionBasedWidgetBinder Interface
};
