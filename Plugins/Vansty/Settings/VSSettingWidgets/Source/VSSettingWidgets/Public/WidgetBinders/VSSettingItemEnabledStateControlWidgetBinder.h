// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBinders/VSWidgetBinder.h"
#include "WidgetControllers/VSSettingItemWidgetMediatorInterface.h"
#include "VSSettingItemEnabledStateControlWidgetBinder.generated.h"

/**
 * 
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

	UFUNCTION(BlueprintCallable, Category = "Enabled State")
	void RefreshEnabledState();

	UFUNCTION(BlueprintNativeEvent, Category = "Enabled State")
	bool GetDesiredEnabledState();
};
