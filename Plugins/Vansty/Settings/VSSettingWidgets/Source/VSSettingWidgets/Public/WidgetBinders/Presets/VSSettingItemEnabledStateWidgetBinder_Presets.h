// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "WidgetBinders/VSSettingItemEnabledStateControlWidgetBinder.h"
#include "VSSettingItemEnabledStateWidgetBinder_Presets.generated.h"

/**
 * 
 */
UCLASS()
class VSSETTINGWIDGETS_API UVSSettingItemEnabledStateWidgetBinder_Presets : public UVSSettingItemEnabledStateControlWidgetBinder
{
	GENERATED_UCLASS_BODY()

protected:
	//~ Begin UVSSettingItemEnabledStateWidgetBinder Interface
	virtual void OnAnySettingItemUpdated_Implementation(UVSSettingItem* SettingItem) override;
	virtual bool GetDesiredEnabledState_Implementation() override;
	
#if WITH_EDITOR
	virtual void EditorRefreshMediator_Implementation() override;
#endif
	//~ Begin UVSSettingItemEnabledStateWidgetBinder Interface


protected:
	UPROPERTY(EditAnywhere, Category = "Enabled State")
	FGameplayTagContainer AutoRefreshUpdatedItems;
};
