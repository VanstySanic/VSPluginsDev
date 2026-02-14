// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VSSettingItemWidgetMediatorInterface.generated.h"

class UVSSettingItemWidgetController;
class UVSSettingItemBase;

// This class does not need to be modified.
UINTERFACE()
class UVSSettingItemWidgetMediatorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VSSETTINGWIDGETS_API IVSSettingItemWidgetMediatorInterface
{
	GENERATED_BODY()

public:
	UVSSettingItemBase* GetSettingItem_Native() const;
	
protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Widget Mediator")
	UVSSettingItemBase* GetSettingItem() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Widget Mediator")
	void OnCurrentSettingItemUpdated();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Widget Mediator")
	void OnAnySettingItemUpdated(UVSSettingItemBase* SettingItem);

	/** Refresh the item's presets. Mostly called when the setting item's tag changes. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Widget Mediator")
	void EditorRefreshMediator();

protected:
	virtual UVSSettingItemBase* GetSettingItem_Implementation() const;
};
