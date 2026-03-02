// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Mediator/Binders/VSWidgetBinder.h"
#include "Mediator/VSSettingItemWidgetMediatorInterface.h"
#include "VSSettingItemEnabledStateControlWidgetBinder.generated.h"

/**
 * Binder that controls enabled/disabled state of the bound "Item" widget.
 */
UCLASS()
class VSSETTINGWIDGETS_API UVSSettingItemEnabledStateControlWidgetBinder : public UVSWidgetBinder, public IVSSettingItemWidgetMediatorInterface
{
	GENERATED_UCLASS_BODY()

protected:
	//~ Begin UVSWidgetBinder Interface
	virtual void Initialize_Implementation() override;
	virtual void BindTypedWidget_Implementation(const FName TypeName, UWidget* Widget) override;
	//~ End UVSWidgetBinder Interface

	/** Re-applies GetDesiredEnabledState to the currently bound "Item" widget. */
	UFUNCTION(BlueprintCallable, Category = "Enabled State")
	void RefreshEnabledState();

	/** Returns whether the target item widget should be enabled. */
	UFUNCTION(BlueprintNativeEvent, Category = "Enabled State")
	bool GetDesiredEnabledState();
};
