// Copyright VanstySanic. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VSSettingItemWidgetMediatorInterface.generated.h"

class UVSSettingItemWidgetController;
class UVSSettingItemBase;

UINTERFACE()
class UVSSettingItemWidgetMediatorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Mediator contract for objects that can resolve and react to a setting item context.
 *
 * Implementers are usually widget controllers and binders in the same mediator chain.
 */
class VSSETTINGWIDGETS_API IVSSettingItemWidgetMediatorInterface
{
	GENERATED_BODY()

public:
	/** Safe native accessor that routes to Execute_GetSettingItem on the backing UObject. */
	UVSSettingItemBase* GetSettingItem_Native() const;
	
protected:
	/** Returns the setting item this mediator currently works with. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Widget Mediator")
	UVSSettingItemBase* GetSettingItem() const;
	
	/** Called when the resolved setting item itself broadcasts update. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Widget Mediator")
	void OnCurrentSettingItemUpdated();

	/** Called when any item in the setting subsystem broadcasts update. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Widget Mediator")
	void OnAnySettingItemUpdated(UVSSettingItemBase* SettingItem);

	/** Editor-only mediator refresh hook (typically used to pull presets from the current item). */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Widget Mediator")
	void EditorRefreshMediator();

protected:
	/** Default implementation resolves through controller or implementing outer mediator object. */
	virtual UVSSettingItemBase* GetSettingItem_Implementation() const;
};
