// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBinders/VSMutableRangedWidgetBinder.h"
#include "WidgetControllers/VSSettingItemWidgetMediatorInterface.h"
#include "VSSettingItemMutableRangedWidgetBinder.generated.h"

/**
 * 
 */
UCLASS()
class VSSETTINGWIDGETS_API UVSSettingItemMutableRangedWidgetBinder : public UVSMutableRangedWidgetBinder, public IVSSettingItemWidgetMediatorInterface
{
	GENERATED_UCLASS_BODY()

protected:
	//~ Begin UVSWidgetBinder Interface
	virtual void Initialize_Implementation() override;
	virtual void BindTypedWidget_Implementation(const FName TypeName, UWidget* Widget) override;
	virtual void UnbindTypedWidget_Implementation(const FName TypeName, UWidget* Widget) override;

	virtual void OnWidgetValueChanged_Implementation(float NewValue) override;
	virtual void OnCurrentSettingItemUpdated_Implementation() override;

#if WITH_EDITOR
	virtual void EditorRefreshMediator_Implementation() override;
#endif
	//~ End UVSWidgetBinder Interface
};
