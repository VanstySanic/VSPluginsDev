// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Mediator/Binders/VSSettingItemEnabledStateControlWidgetBinder.h"
#include "VSSettingItemEnabledStateControlWidgetBinder_CommonPresets.generated.h"

/**
 * Common preset enabled-state policy driven by built-in setting dependencies.
 */
UCLASS()
class VSSETTINGWIDGETS_API UVSSettingItemEnabledStateControlWidgetBinder_CommonPresets : public UVSSettingItemEnabledStateControlWidgetBinder
{
	GENERATED_UCLASS_BODY()

protected:
	//~ Begin UVSSettingItemEnabledStateControlWidgetBinder Interface
	virtual void OnAnySettingItemUpdated_Implementation(UVSSettingItemBase* SettingItem) override;
	virtual bool GetDesiredEnabledState_Implementation() override;
	
#if WITH_EDITOR
	virtual void EditorRefreshMediator_Implementation() override;
#endif
	//~ End UVSSettingItemEnabledStateControlWidgetBinder Interface


protected:
	/** Item tags that should trigger RefreshEnabledState when they update. */
	UPROPERTY(EditAnywhere, Category = "Enabled State")
	FGameplayTagContainer AutoRefreshUpdatedItems;
};
