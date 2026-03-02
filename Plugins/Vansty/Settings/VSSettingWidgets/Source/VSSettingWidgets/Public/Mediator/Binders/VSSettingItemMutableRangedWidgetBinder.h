// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Mediator/Binders/VSMutableRangedWidgetBinder.h"
#include "Mediator/VSSettingItemWidgetMediatorInterface.h"
#include "VSSettingItemMutableRangedWidgetBinder.generated.h"

/**
 * Mutable range binder adapter that supports value + mute state synchronization.
 */
UCLASS()
class VSSETTINGWIDGETS_API UVSSettingItemMutableRangedWidgetBinder : public UVSMutableRangedWidgetBinder, public IVSSettingItemWidgetMediatorInterface
{
	GENERATED_UCLASS_BODY()

protected:
	//~ Begin UVSWidgetBinder Interface
	virtual void BindTypedWidget_Implementation(const FName TypeName, UWidget* Widget) override;
	virtual void UnbindTypedWidget_Implementation(const FName TypeName, UWidget* Widget) override;
	virtual void OnCurrentSettingItemUpdated_Implementation() override;
	
	virtual float GetExternalNonMutedValue_Implementation() const override;
	virtual bool GetExternalIsMuted_Implementation() const override;
	virtual void OnWidgetValueChanged_Implementation(float NewValue) override;
	
#if WITH_EDITOR
	virtual void EditorRefreshMediator_Implementation() override;
#endif
	//~ End UVSWidgetBinder Interface
};
