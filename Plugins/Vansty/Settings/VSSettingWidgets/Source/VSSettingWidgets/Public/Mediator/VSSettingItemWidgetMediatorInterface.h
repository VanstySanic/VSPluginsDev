// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VSSettingItemWidgetMediatorInterface.generated.h"

class UVSSettingItemWidgetController;
class UVSSettingItem;

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
	UVSSettingItem* GetSettingItem_Native() const;
	
protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Widget Mediator")
	UVSSettingItem* GetSettingItem() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Widget Mediator")
	void OnCurrentSettingItemUpdated();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Widget Mediator")
	void OnAnySettingItemUpdated(UVSSettingItem* SettingItem);

	/** Refresh the item's presets. Mostly called when the setting item's tag changes. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Widget Mediator")
	void EditorRefreshMediator();

protected:
	virtual UVSSettingItem* GetSettingItem_Implementation() const;
};
